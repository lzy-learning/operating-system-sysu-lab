#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;
// void first_thread(void*){
//     int assign_block = 1024;
//     int virtual_start_addr = memoryManager.allocatePages(AddressPoolType::KERNEL, assign_block);
//     printf("virtual address: 0x%x\n", virtual_start_addr);
//     int pde, pte, physical_addr;
//     for(int i = 0;i<assign_block;i++){
//         if(i == 767){
//             printf("breakpoints\n");
//         }
//         pde = memoryManager.toPDE((const int)virtual_start_addr);
//         pte = memoryManager.toPTE((const int)virtual_start_addr);
//         physical_addr = memoryManager.vaddr2paddr(virtual_start_addr);
        
//         printf("virtual address: %x => physical address: %x\n", virtual_start_addr, physical_addr);
//         printf("pde: %x, pte: %x\n", pde, pte);
//         printf("\n");
//         if(pde > 0xfffffc00){
//             printf("%d\n", i);
//             break;
//         }
//         virtual_start_addr += 4096;
//     }
//     asm_halt();
// }
// void first_thread(void *arg)
// {
//     // 第1个线程不可以返回
//     // stdio.moveCursor(0);
//     // for (int i = 0; i < 25 * 80; ++i)
//     // {
//     //     stdio.print(' ');
//     // }
//     // stdio.moveCursor(0);

//     char *p1 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 100);
//     char *p2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 10);
//     char *p3 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 100);

//     printf("%x %x %x\n", p1, p2, p3);

//     printf("p1 vaddr: 0x%x, paddr: 0x%x\n", (int)p1, memoryManager.vaddr2paddr((const int)p1));
//     printf("p1 pde: 0x%x, pte: 0x%x\n", memoryManager.toPDE((int)p1), memoryManager.toPTE((int)p1));

//     memoryManager.releasePages(AddressPoolType::KERNEL, (int)p2, 10);
//     p2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 100);

//     printf("%x\n", p2);

//     p2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 10);
    
//     printf("%x\n", p2);

//     asm_halt();
// }

void first_thread(void*){
    int vaddr, pde, last_pde=0xfffffc00, start_vaddr;
    int allocate_page = 0;
    for(int i = 0;i<4096;i++){
        vaddr = memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
        pde = memoryManager.toPDE(vaddr);
        
        if(pde > last_pde){
            printf("allocated %d pages and pde change from 0x%x to 0x%x\n", allocate_page, last_pde, pde);
            start_vaddr = vaddr-allocate_page*PAGE_SIZE;
            printf("start virtual address: 0x%x  =>  physical address: 0x%x\n", start_vaddr, memoryManager.vaddr2paddr(start_vaddr));
            printf("======================================================\n");
            last_pde = pde;
            allocate_page = 0;
        }

        allocate_page += 1;
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

    // 内存管理器
    memoryManager.openPageMechanism();
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
