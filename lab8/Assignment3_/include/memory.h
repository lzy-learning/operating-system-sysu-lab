#ifndef MEMORY_H
#define MEMORY_H

#include "address_pool.h"
#include "os_constant.h"
#include "stdio.h"
#include "asm_utils.h"
#include "stdlib.h"
#include "list.h"

struct FCB
{
    int virtualAddress;
    int count;
    int pid;
    ListItem tagInList;
};


// 将内存分为内核空间和用户空间，用这个枚举类型标明是用户申请还是内核申请
enum AddressPoolType{
    USER,
    KERNEL
};

class MemoryManager{
public:
    // 管理FCB
    // FCB* fcb_sets;
    // bool* fcb_status;
    // 可管理的内存容量
    int totalMemory;
    // 内核物理地址池
    AddressPool kernelPhysical;
    // 用户物理地址池
    AddressPool userPhysical;
    // 内核虚拟地址池
    AddressPool kernelVirtual;
    // 用户虚拟地址池放在每个进程的PCB中

    // 记录分配给每次分配的信息
    List allocate_list;

    MemoryManager();

    // 初始化地址池
    void initialize();

    // 分配若干个虚拟页
    int allocateVirtualPages(enum AddressPoolType type, const int count);

    // 从type类型的物理地址池中分配count个连续的页
    // 成功，返回起始地址；失败，返回0
    int allocatePhysicalPages(enum AddressPoolType type, const int count);

    // 释放从paddr开始的count个物理页
    void releasePhysicalPages(enum AddressPoolType type, const int paddr, const int count);

    // 获取内存总容量
    int getTotalMemory();

    // 开启分页机制(建立分层页表)
    void openPageMechanism();

    // 根据虚拟地址构造页目录项和页表项
    int toPDE(const int virtualAddress);
    int toPTE(const int virtualAddress);

    // 找到虚拟页对应的物理页的物理地址
    int vaddr2paddr(int vaddr);

    // 实现虚拟地址和物理地址的连接
    // 具体步骤：根据虚拟地址得到页目录项和页表项，页目录项如果已经指向一个页表就无需分配，否则分配一个页表并设置页表内偏移
    // 设置页表项指向物理页
    bool connectPhysicalVirtualPage(const int virtualAddress, const int physicalAddress);

    // 在开启二级分页后分配页，步骤是先分配若干虚拟页和若干物理页，将这几个页联系起来，虚拟页是连续的，而物理页不需要连续
    int allocatePages(enum AddressPoolType type, const int count);

    // 释放页内存，先释放对应的物理页，然后释放虚拟页
    void releasePages(enum AddressPoolType type, const int virtualAddress, const int count);

    // 仅释放虚拟页
    void releaseVirtualPages(enum AddressPoolType type, const int virtualAddress, const int count);

    // 分配一个FCB
    FCB* allocateFCB();
};
#endif