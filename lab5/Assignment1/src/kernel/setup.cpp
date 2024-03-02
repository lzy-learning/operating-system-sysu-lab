#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;


extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();
    // 屏幕IO处理部件
    stdio.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    // asm_enable_interrupt();
    printf("print percentage: %%\n"
           "print char: %c\n"
           "print string: %s\n"
           "print decimal : %d\n"
           "print hexadecimal: %x\n",
           'L', "linzhiyang", 21307403, 0x55555);
    asm_halt();
}

