#include "syscall.h"
#include "stdlib.h"
#include "os_modules.h"
#include "asm_utils.h"
#include "interrupt.h"

int system_call_table[MAX_SYSTEM_CALL];
SystemService::SystemService(){
    this->initialize();
}

void SystemService::initialize(){
    memset((char*)system_call_table, 0, sizeof(int)*MAX_SYSTEM_CALL);
    // 注意中断描述符的DPL=3，这样用户态程序才能使用这个中断描述符
    interruptManager.setInterruptDescriptor(0x80, (uint32)asm_system_call_handler, 3);
}

bool SystemService::setSystemCall(int index, int function){
    system_call_table[index] = function;
    return true;
}

int fork(){
    return asm_system_call(2);
}

int syscall_fork(){
    return programManager.fork();
}