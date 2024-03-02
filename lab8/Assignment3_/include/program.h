#ifndef PROGRAM_H
#define PROGRAM_H

#include "list.h"
#include "thread.h"

#define ListItem2PCB(ADDRESS, LIST_ITEM) ((PCB *)((int)(ADDRESS) - (int)&((PCB *)0)->LIST_ITEM))

class ProgramManager
{
public:
    List allPrograms;   // 所有状态的线程/进程的队列
    List readyPrograms; // 处于ready(就绪态)的线程/进程的队列
    PCB *running;       // 当前执行的线程
    int USER_CODE_SELECTOR; // 用户代码段选择子
    int USER_DATA_SELECTOR; // 用户数据段选择子
    int USER_STACK_SELECTOR;    // 用户栈段选择子
    
public:
    ProgramManager();
    void initialize();
    void initializeTSS();

    // 创建一个线程并放入就绪队列

    // function：线程执行的函数
    // parameter：指向函数的参数的指针
    // name：线程的名称
    // priority：线程的优先级

    // 成功，返回pid；失败，返回-1
    int executeThread(ThreadFunction function, void *parameter, const char *name, int priority);

    
    // 分配一个PCB
    PCB *allocatePCB();
    // 归还一个PCB
    // program：待释放的PCB
    void releasePCB(PCB *program);

    // 执行线程调度
    void schedule();

    // MESA模型，当一个线程被唤醒时它不会立即执行，而是加入到就绪队列尾
    void MESA_Wakeup(PCB*);

    // 创建一个进程，返回进程pid
    int executeProcess(const char* filename, int priority);
    // 初始化进程的页目录表
    int createProcessPageDirectory();
    // 为进程创建虚拟地址池
    bool createUserVirtualPool(PCB* process);

    void activateProgramPage(PCB* program);

    // 创建子进程，复制当前父进程的所有资源
    int fork();

    // 复制父进程的资源到子进程
    bool copyProcess(PCB* parent, PCB* child);
};

void program_exit();
void load_process(const char *filename);

#endif