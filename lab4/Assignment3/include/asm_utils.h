#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include "os_type.h"

extern "C" void asm_my_info();
extern "C" void asm_lidt(uint32 start, uint16 limit);
// 定义在asm_utils.asm中，是中断处理程序
extern "C" void asm_unhandled_interrupt();
// 定义在asm_utils.asm中，处理除0的中断处理程序
extern "C" void divided_by_zero();
extern "C" void asm_halt();

#endif