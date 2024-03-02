#include "program.h"
#include "os_modules.h"
#include "stdlib.h"
#include "stdio.h"
#include "list.h"
#include "thread.h"
#include "asm_utils.h"

// PCB大小
const int PCB_SIZE = 4096;
// 存放PCB块的数组，一开始就被初始化好
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT];
// 标识PCB块有没有被使用
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];

// 一个线程退出后，线程调度算法会选择下一个线程执行，但是0进程不能退出
// 所以可以看到当thread->pid==0时，系统关中断并进入死循环
void program_exit()
{
    PCB *thread = programManager.running;
    thread->status = ProgramStatus::DEAD;

    if (thread->pid)
    {
        programManager.schedule();
    }
    else
    {
        interruptManager.disableInterrupt();
        printf("halt\n");
        asm_halt();
    }
}
ProgramManager::ProgramManager(){
    initialize();
}

void ProgramManager::initialize(){
    allPrograms.initialize();
    readyPrograms.initialize();
    running = nullptr;
    for(int i = 0;i<MAX_PROGRAM_AMOUNT;i++)PCB_SET_STATUS[i]=false;
}

PCB* ProgramManager::allocatePCB(){
    // 找到第一个没有分配的PCB并返回这个PCB的地址
    for(int i = 0;i<MAX_PROGRAM_AMOUNT;i++){
        if(!PCB_SET_STATUS[i]){
            PCB_SET_STATUS[i] = true;
            // 第i个PCB的首地址
            return (PCB*)((int)PCB_SET+PCB_SIZE*i);
        }
    }
    return nullptr;
}
void ProgramManager::releasePCB(PCB* program){
    // 某个PCB的地址减去PCB数组的首地址，再除以PCB大小即可得到这个PCB的索引
    int idx = ((int)program - (int)PCB_SET)/PCB_SIZE;
    PCB_SET_STATUS[idx] = false;
}
int ProgramManager::executeThread(ThreadFunction func, void* param, const char* name, int priority){
    // 获得当前中断状态，用于后面恢复
    bool status = interruptManager.getInterruptStatus();
    // 先关中断，防止创建线程的过程被中断
    interruptManager.disableInterrupt();

    // 分配一页作为PCB
    PCB* thread = allocatePCB();
    if(thread == nullptr){
        printf("failed allocate\n");
    }

    // 初始化这段内存空间，大小4096字节
    memset(thread, 0, PCB_SIZE);

    // 设置PCB属性
    // 栈指针
    thread->stack = (int*)((int)thread+PCB_SIZE);
    thread->stack -= 7;
    // 下面4个0分别代表ebp ebx edi esi
    thread->stack[0] = 0;
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    // 下面分别是线程函数，线程返回地址(即线程退出函数，在里面执行线程调度算法)，线程参数地址
    thread->stack[4] = (int)func;
    thread->stack[5] = (int)program_exit;
    thread->stack[6] = (int)param;

    // 线程名
    for(int i = 0;i<MAX_PROGRAM_NAME && name[i]!='\0';i++){
        thread->name[i] = name[i];
    }
    // 线程状态
    thread->status = ProgramStatus::READY;
    // 线程优先级  
    thread->priority = priority;
    // 线程序号
    thread->pid = ((int)thread-(int)PCB_SET)/PCB_SIZE;
    // 线程时间片总时间
    thread->ticks = priority*10;
    // 线程已执行时间，以实时钟中断为单位
    thread->ticksPassedBy = 0;
    
    allPrograms.push_back(&(thread->tagInAllList));
    readyPrograms.push_back(&(thread->tagInGeneralList));

    // 恢复中断
    interruptManager.setInterruptStatus(status);
    return thread->pid;
}

void ProgramManager::schedule(){
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if(readyPrograms.size() == 0){
        interruptManager.setInterruptStatus(status);
        return;
    }
    if(running->status == ProgramStatus::RUNNING){
        // 将当前线程放入就绪队列
        running->status = ProgramStatus::READY;
        running->ticks = running->priority*10;
        readyPrograms.push_back(&(running->tagInGeneralList));
    }
    // 如果线程已经完成则将对应PCB销毁
    else if(running->status == ProgramStatus::DEAD){
        releasePCB(running);
    }

    // 选择下一个线程
    ListNode* node = readyPrograms.front();
    PCB* next = ListNode2PCB(node, tagInGeneralList);
    PCB* cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms.pop_front();

    // 切换线程
    asm_switch_thread(cur, next);
    interruptManager.setInterruptStatus(status);
}