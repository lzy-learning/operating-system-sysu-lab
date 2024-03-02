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


// int allocateVirtualPages(enum AddressPoolType type, const int count);
// int allocatePhysicalPages(enum AddressPoolType type, const int count);
// void releasePhysicalPages(enum AddressPoolType type, const int paddr, const int count);
// int getTotalMemory();
// void openPageMechanism();
// int toPDE(const int virtualAddress);
// int toPTE(const int virtualAddress);
// int vaddr2paddr(int vaddr);
// bool connectPhysicalVirtualPage(const int virtualAddress, const int physicalAddress);
// int allocatePages(enum AddressPoolType type, const int count);
// void releasePages(enum AddressPoolType type, const int virtualAddress, const int count);
// void releaseVirtualPages(enum AddressPoolType type, const int virtualAddress, const int count);

void first_thread(void*){
    int assign_block = 7;
    int vaddr;
    for(int i=0;i<assign_block;i++){
        vaddr = memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
        printf("virtual address: 0x%x => physical address: 0x%x\n",
            vaddr, memoryManager.vaddr2paddr(vaddr));
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
