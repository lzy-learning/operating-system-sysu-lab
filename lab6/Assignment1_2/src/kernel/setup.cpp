#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 互斥锁
Mutex mutex;

// 自旋锁
SpinLock lock_cheese_burger;

// 信号量
Semaphore sem_cheese_burger;

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

int cheese_burger;

void a_mother(void *arg)
{
    int delay = 0;

    printf("mom: start to make cheese burger, there are %d cheese burger now\n", cheese_burger);
    // make 10 cheese_burger
    cheese_burger += 10;

    printf("mom: oh, I have to hang clothes out.\n");
    mutex.lock();
    printf("mom get lock\n");
    // hanging clothes out
    delay = 0x11111111;
    while (delay)
        --delay;
    // done
    printf("mom: Oh, Jesus! There are %d cheese burgers\n", cheese_burger);
    printf("mom unlock\n");
    mutex.unlock();
}

void a_naughty_boy(void *arg)
{
    mutex.lock();
    printf("lzy get lock\n");
    printf("lzy: Look what I found!\n");
    // eat all cheese_burgers out secretly
    cheese_burger -= 10;
    // run away as fast as possible
    printf("lzy release lock\n");
    mutex.unlock();
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

    cheese_burger = 0;
    
    programManager.executeThread(a_mother, nullptr, "second thread", 1);
    programManager.executeThread(a_naughty_boy, nullptr, "third thread", 1);

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

    mutex.initialize();

    // 初始化锁
    lock_cheese_burger.initialize();

    // 初始化信号量
    sem_cheese_burger.initialize(1);

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
