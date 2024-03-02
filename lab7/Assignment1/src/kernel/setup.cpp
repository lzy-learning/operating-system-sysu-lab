#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "os_constant.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理模块
MemoryManager memoryManager;

void first_thread(void*){
    int start_paddr = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 10);
    printf("start physical address: 0x%x\n", start_paddr);
    int my_id = 21307403;
    ((int*)start_paddr)[0] = 21307403;
    printf("write my id %d to this address 0x%x\n", 21307403, start_paddr);
    printf("view the value in the address 0x%x: %d\n", start_paddr, *((int*)start_paddr));

    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, start_paddr, 10);
    printf("release the physical page.\n");
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
    stdio.clearScreen();

    // 进程/线程管理器
    programManager.initialize();

    // 内存管理器
    memoryManager.initialize();

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
