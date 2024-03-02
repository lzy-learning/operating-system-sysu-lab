#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include "os_type.h"

extern "C" void asm_delay();    // 延迟输出，不然太快看不出跑马灯的效果
extern "C" void asm_hello_world();
extern "C" void asm_lidt(uint32 start, uint16 limit);
extern "C" void asm_unhandled_interrupt();      // 没有中断处理程序的中断会执行该函数
extern "C" void asm_halt();     // jmp $
extern "C" void asm_out_port(uint16 port, uint8 value);
extern "C" void asm_in_port(uint16 port, uint8 *value);
extern "C" void asm_enable_interrupt();
extern "C" void asm_time_interrupt_handler();

#endif