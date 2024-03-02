#ifndef SYSCALL_H
#define SYSCALL_H

#include "os_constant.h"

class SystemService{
public:
    SystemService();
    void initialize();
    // 设置系统调用
    bool setSystemCall(int index, int function);
};

int syscall_0(int first, int second, int third, int forth, int fifth);

// 系统调用fork
int fork();
int syscall_fork();

#endif