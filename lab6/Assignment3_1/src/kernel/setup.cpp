#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 信号量
Semaphore sem_chopsticks[5];

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;


void philosopher(void* arg){
    int id = (int)arg;
    int loop_times = 3;
    while(loop_times--){
        sem_chopsticks[id].P();
        sem_chopsticks[(id+1)%5].P();
        if(id == 1)
            printf("Philosopher %d and 7403 is eating\n", id);
        else printf("Philosopher %d is eating\n", id);
        int delay = 0x11111111;
        while(delay--);

        sem_chopsticks[id].V();
        sem_chopsticks[(id+1)%5].V();

        if (id == 1)
            printf("Philosopher %d and 7403 is thinking\n", id);
        else printf("Philosopher %d is thinking\n", id);
        delay = 0x11111111;
        while(delay--);
    }

    printf("Philosopher %d exit\n", id);
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    for(int i = 0;i<5;i++){
        programManager.executeThread(philosopher, (void*)i, "philosopher", 1);
    }

    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 初始化信号量，一共5根筷子
    for(int i = 0;i<5;i++)sem_chopsticks[i].initialize(1);

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
