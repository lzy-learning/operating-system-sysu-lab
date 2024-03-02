#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#define MAX_CAKE_NUM 5  // 最多容纳5块蛋糕

int num_mango_cake = 0; // 当前盘子上的芒果蛋糕数
int num_matcha_cake = 0;    // 盘子上的抹茶蛋糕数
int need_mango = 0;     // 需要的芒果蛋糕数
int need_matcha = 0;    // 需要的抹茶蛋糕数
Semaphore male_sem;
Semaphore female_sem;
Semaphore num_matcha_sem;
Semaphore num_mango_sem;

void waiter_A(void* arg){
    while(1){
        if(need_matcha > 0 && num_mango_cake+num_matcha_cake < MAX_CAKE_NUM){
            num_matcha_sem.P();
            num_matcha_cake+=1;
            need_matcha-=1;
            num_matcha_sem.V();
            printf("Waiter-A 7403 put a piece of matcha cake in the plate\n");
            male_sem.V();
        }
    }
    printf("Waiter-A produce over\n");
}
void waiter_B(void* arg){
    while(1){
        // 盘子没满则放入芒果蛋糕
        if(need_mango > 0 && num_mango_cake+num_matcha_cake < MAX_CAKE_NUM){
            num_mango_sem.P();
            num_mango_cake+=1;
            need_mango-=1;
            num_mango_sem.V();
            printf("Waiter-B 7403 put a piece of mongo cake in the plate\n");
            female_sem.V();
        }
    }
    printf("Waiter-B produce over\n");
}
void guest_male(void* arg){
    int id = (int)arg;
    while(1){
        if(num_matcha_cake>0){
            num_matcha_sem.P();
            num_matcha_cake-=1;
            num_matcha_sem.V();
            printf("Male guest %d consume a piece of matcha cake in the plate\n", id);
            programManager.schedule();
        }
        else if(num_matcha_cake+num_mango_cake<MAX_CAKE_NUM){
            need_matcha+=1;
            printf("Male guest %d is waiting to eat\n", id);
            male_sem.P();
        }
        else{
            printf("The plate is full\n");
        }
    }
    printf("Male guest %d consume over\n", id);
}
void guest_female(void* arg){
    int id = (int)arg;
    while(1){
        if(num_mango_cake>0){
            num_mango_sem.P();
            num_mango_cake-=1;
            num_mango_sem.V();
            printf("Female guest %d consume a piece of mango cake in the plate\n", id);
            programManager.schedule();
        }
        else if(num_mango_cake+num_matcha_cake<MAX_CAKE_NUM){
            need_mango+=1;
            printf("Female guest %d is waiting to eat\n", id);
            female_sem.P();
        }
        else{
            printf("The plate is full\n");
        }
    }
    printf("Female guest %d consume over\n", id);
}

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;


void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 服务生A放入抹茶蛋糕
    programManager.executeThread(waiter_A, nullptr, "Waiter-A", 1);
    // 服务生B放入芒果蛋糕
    programManager.executeThread(waiter_B, nullptr, "Waiter-B", 1);

    // 6名男性来宾，享用抹茶蛋糕
    for(int i = 1;i<=6;i++){
        programManager.executeThread(guest_male, (void*)i, "Male-guest", 1);
    }
    // 4名女性来宾，享用芒果蛋糕
    for(int i = 1;i<=4;i++){
        programManager.executeThread(guest_female, (void*)i, "Female-guest", 1);
    }
    
    while(1){
        // 打印共享变量的值
        printf("matcha num: %d, mongo num: %d\n", num_matcha_cake, num_mango_cake);
        printf("matcha need: %d, mongo need: %d\n", need_matcha, need_mango);
        printf("===========================================\n");
        programManager.schedule();
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

    male_sem.initialize(0);
    female_sem.initialize(0);
    num_mango_sem.initialize(1);
    num_matcha_sem.initialize(1);

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
