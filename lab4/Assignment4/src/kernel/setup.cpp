#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "marquee.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;

char info[] = "21307403linzhiyang";
Marquee marquee;


extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();
    // 跑马灯类
    marquee.initialize(info, (uint32)18, 12, 0);
    // 屏幕IO处理部件
    stdio.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    asm_enable_interrupt();
    asm_halt();
}

