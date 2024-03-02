#ifndef MEMORY_H
#define MEMORY_H

#include "address_pool.h"
#include "os_constant.h"
#include "stdio.h"
#include "asm_utils.h"
#include "stdlib.h"

// 将内存分为内核空间和用户空间，用这个枚举类型标明是用户申请还是内核申请
enum AddressPoolType{
    USER,
    KERNEL
};

class MemoryManager{
public:
    // 可管理的内存容量
    int totalMemory;
    // 内核物理地址池
    AddressPool kernelPhysical;
    // 用户物理地址池
    AddressPool userPhysical;

    MemoryManager();

    // 初始化地址池
    void initialize();

    // 从type类型的物理地址池中分配count个连续的页
    // 成功，返回起始地址；失败，返回0
    int allocatePhysicalPages(enum AddressPoolType type, const int count);

    // 释放从paddr开始的count个物理页
    void releasePhysicalPages(enum AddressPoolType type, const int paddr, const int count);

    // 获取内存总容量
    int getTotalMemory();

    // 开启分页机制(建立分层页表)
    void openPageMechanism();
};
#endif