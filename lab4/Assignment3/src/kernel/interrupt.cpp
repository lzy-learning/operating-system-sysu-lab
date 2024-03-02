#include "interrupt.h"
#include "os_type.h"
#include "os_constant.h"
#include "asm_utils.h"

InterruptManager::InterruptManager()
{
    initialize();
}

void InterruptManager::initialize()
{
    // 初始化IDT，IDT_START_ADDRESS为0x8880
    IDT = (uint32 *)IDT_START_ADDRESS;
    // 调用汇编函数将IDT初始地址载入IDTR寄存器中
    asm_lidt(IDT_START_ADDRESS, 256 * 8 - 1);

    setInterruptDescriptor(0, (uint32)divided_by_zero, 0);
    for (uint i = 1; i < 256; ++i)
    {
        setInterruptDescriptor(i, (uint32)asm_unhandled_interrupt, 0);
    }

}

void InterruptManager::setInterruptDescriptor(uint32 index, uint32 address, byte DPL)
{
    // 中断描述符的低32位，分别为目标代码段选择子和中断处理程序的偏移地址的低16位
    IDT[index * 2] = (CODE_SELECTOR << 16) | (address & 0x0000ffff);
    // 中断描述符的高32位，高16位为中断处理程序的偏移地址的高16位，然后是P位(段存在位)、DPL(特权级描述符)等
    IDT[index * 2 + 1] = (address & 0xffff0000) | (0x1 << 15) | (DPL << 13) | (0xe << 8);
}
