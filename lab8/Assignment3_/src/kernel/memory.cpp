#include "memory.h"
#include "os_modules.h"

#define MAX_FCB_COUNT 16000
FCB fcb_sets[MAX_FCB_COUNT];
bool fcb_status[MAX_FCB_COUNT];

MemoryManager::MemoryManager() {
    initialize();
}

int MemoryManager::getTotalMemory()
{

    if(!this->totalMemory)
    {
        int memory = *((int *)MEMORY_SIZE_ADDRESS);
        // ax寄存器保存的内容
        int low = memory & 0xffff;
        // bx寄存器保存的内容
        int high = (memory >> 16) & 0xffff;

        this->totalMemory = low * 1024 + high * 64 * 1024;
    }

    return this->totalMemory;
}
void MemoryManager::initialize()
{
    // this->openPageMechanism();
    this->allocate_list.initialize();

    this->totalMemory = 0;
    this->totalMemory = getTotalMemory();

    // 预留的内存
    int usedMemory = 256 * PAGE_SIZE + 0x100000;
    if (this->totalMemory < usedMemory)
    {
        printf("memory is too small, halt.\n");
        asm_halt();
    }
    // 剩余的空闲的内存
    int freeMemory = this->totalMemory - usedMemory;

    int freePages = freeMemory / PAGE_SIZE;
    int kernelPages = freePages / 2;
    int userPages = freePages - kernelPages;

    int kernelPhysicalStartAddress = usedMemory;
    int userPhysicalStartAddress = usedMemory + kernelPages * PAGE_SIZE;

    int kernelPhysicalBitMapStart = BITMAP_START_ADDRESS;
    int userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages, 8);
    
    // 这里内核虚拟池的位图起始地址紧接用户物理池位图
    int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages, 8);

    kernelPhysical.initialize(
        (char *)kernelPhysicalBitMapStart,
        kernelPages,
        kernelPhysicalStartAddress);

    userPhysical.initialize(
        (char *)userPhysicalBitMapStart,
        userPages,
        userPhysicalStartAddress);

    // KERNEL_VIRTUAL_START => 0xc0100000
    kernelVirtual.initialize(
        (char *)kernelVirtualBitMapStart,
        kernelPages,
        KERNEL_VIRTUAL_START);

    printf("total memory: %d bytes ( %d MB )\n",
           this->totalMemory,
           this->totalMemory / 1024 / 1024);

    printf("kernel pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bitmap start address: 0x%x\n",
           kernelPhysicalStartAddress,
           kernelPages, kernelPages * PAGE_SIZE / 1024 / 1024,
           kernelPhysicalBitMapStart);

    printf("user pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bit map start address: 0x%x\n",
           userPhysicalStartAddress,
           userPages, userPages * PAGE_SIZE / 1024 / 1024,
           userPhysicalBitMapStart);

    printf("kernel virtual pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d  ( %d MB ) \n"
           "    bit map start address: 0x%x\n",
           KERNEL_VIRTUAL_START,
           userPages, kernelPages * PAGE_SIZE / 1024 / 1024,
           kernelVirtualBitMapStart);
    
    // 分配FCB的空间
    // int fcb_sets_pages = ceil(sizeof(FCB)*MAX_FCB_COUNT, PAGE_SIZE);
    // int fcb_status_pages = ceil(sizeof(bool)*MAX_FCB_COUNT, PAGE_SIZE);
    // this->fcb_sets = (FCB*)(this->allocatePages(AddressPoolType::KERNEL, fcb_sets_pages));
    // this->fcb_status = (bool*)(this->allocatePages(AddressPoolType::KERNEL, fcb_status_pages));
    // 初始化描述fcb分配状态的列表
    for(int i = 0;i<MAX_FCB_COUNT;i++)fcb_status[i] = false;
}

int MemoryManager::allocateVirtualPages(enum AddressPoolType type, const int count)
{
    int start = -1;

    if (type == AddressPoolType::KERNEL)
    {
        start = kernelVirtual.allocate(count);
    }
    else if(type == AddressPoolType::USER){
        // 用户申请，由PCB内的用户虚拟地址池去分配
        start = programManager.running->userVirtual.allocate(count);
    }
    return (start == -1) ? 0 : start;
}

int MemoryManager::allocatePhysicalPages(enum AddressPoolType type, const int count)
{
    int start = -1;

    if (type == AddressPoolType::KERNEL)
    {
        start = kernelPhysical.allocate(count);
    }
    else if (type == AddressPoolType::USER)
    {
        start = userPhysical.allocate(count);
    }
    return (start == -1) ? 0 : start;
}


void MemoryManager::openPageMechanism(){
    /*
    页目录表和页表都是1024大小，表项也都是4B
    */
    // 页目录表首地址：0x100000
    int *directory = (int*)PAGE_DIRECTORY;
    // 页表首地址
    int *page = (int*)(PAGE_DIRECTORY+PAGE_SIZE);

    // 初始化页表
    memset(page, 0, PAGE_SIZE);
    int address = 0;
    // 设置页表项的低三位以及映射到的物理地址
    // 比如page[0]就映射到0号物理页，page[1]就映射到1号物理页
    // 这里映射到物理地址的0-1MB，即内核所在的物理内存处
    for (int i = 0; i < 256; ++i)
    {
        // 特权级位U/S = 1, 读写位R/W = 1, 存在位P = 1
        page[i] = address | 0x7;
        address += PAGE_SIZE;   // 这里相当于页表项的高20位不断加1  
    }

    // 初始化页目录表
    memset(directory, 0, PAGE_SIZE);
    directory[0] = ((int)page)|0x07;
    // 3GB的内核空间
    directory[768] = directory[0];
    // 最后一个页目录项指向页目录表，这里是为了在开启二级分页机制后程序员能够方便主动设置页目录表和页表
    directory[1023] = ((int)directory) | 0x7;

    // 初始化cr3，cr0，开启分页机制
    asm_init_page_reg(directory);

    printf("open page mechanism\n");

}

int MemoryManager::toPDE(const int virtualAddress){
    return (0xfffff000 + (((virtualAddress & 0xffc00000) >> 22) * 4));
}

int MemoryManager::toPTE(const int virtualAddress){
    return (0xffc00000 + ((virtualAddress & 0xffc00000) >> 10) + (((virtualAddress & 0x003ff000) >> 12) * 4));
}

bool MemoryManager::connectPhysicalVirtualPage(const int virtualAddress, const int physicalAddress){
    int *pde = (int*)toPDE(virtualAddress);
    int *pte = (int*)toPTE(virtualAddress);

    // 如果页目录项没有对应页表，则分配一个页表
    if(!(*pde)){
        int page = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
        if(!page){
            // printf("error happend in func allocatePhysicalPages");
            return false;
        }

        // 让页目录项指向页表
        *pde = page | 0x7;
        // 初始化页表
        char* pagePtr = (char*)(((int)pte) & 0xfffff000);
        memset(pagePtr, 0, PAGE_SIZE);
    }

    // 让页表项指向物理页
    *pte = physicalAddress | 0x7;

    return true;
}

int MemoryManager::allocatePages(enum AddressPoolType type, const int count){
    // 1、从虚拟地址池中分配若干连续的虚拟页并得到首地址
    int virtualAddress = allocateVirtualPages(type, count);
    if(!virtualAddress)return 0;

    bool flag;
    int physicalAddress;
    int vaddr = virtualAddress; // 保留一份副本，方便分配过程中发现空间不足可以清除已分配的空间以及返回首地址

    // 依次为每个虚拟页指定一个物理页，因为虚拟页是连续的，所以每次分配完加上页大小就是下一个虚拟页
    for(int i = 0;i<count;i++, vaddr += PAGE_SIZE){
        flag = false;
        // 2、从物理地址池分配一个物理页
        physicalAddress = allocatePhysicalPages(type, 1);
        if(physicalAddress){
            // 3、为虚拟地址建立页目录项和页表项，使CPU可以根据虚拟地址得到对应物理地址
            flag = connectPhysicalVirtualPage(vaddr, physicalAddress);
        }
        else {
            // printf("fail to allocate physical page\n");
            flag = false;
        }
        // 如果分配失败
        if(!flag){
            // 回收前i个已经指定了物理页的虚拟页
            releasePages(type, virtualAddress, i);
            // 回收剩余未指定物理页的虚拟页
            releaseVirtualPages(type, virtualAddress+i*PAGE_SIZE, count-i);

            // 按照FIFO置换页
            ListItem* tag = this->allocate_list.front();
            this->allocate_list.pop_front();
            if(tag == nullptr)return 0;
            int k;
            for(k = 0;k<MAX_FCB_COUNT;k++)
                if(fcb_status[k] && &(fcb_sets[k].tagInList) == tag)
                    break;
            // 将内存的这页调入磁盘，这里用释放作为演示
            fcb_status[k] = false;
            int paddr = this->vaddr2paddr(fcb_sets[k].virtualAddress);
            releasePages(type, fcb_sets[k].virtualAddress, fcb_sets[k].count);
            // printf("Swap the page with physical address 0x%x to the disk.\n", paddr);
            // 重新进行分配
            virtualAddress = allocateVirtualPages(type, count);
            vaddr = virtualAddress-PAGE_SIZE;
            if(!virtualAddress)return 0;
            i = -1;
            continue;
        }
    }
    // 分配成功之后保存FCB数据
    FCB* fcb = allocateFCB();
    fcb->virtualAddress = virtualAddress;
    fcb->count = count;
    fcb->pid = type == AddressPoolType::KERNEL ? 0 : 1;
    this->allocate_list.push_back(&fcb->tagInList);

    return virtualAddress;
}

void MemoryManager::releasePages(enum AddressPoolType type, const int virtualAddress, const int count){
    int vaddr = virtualAddress;
    int *pte, *pde;

    bool flag;
    const int entry_num = PAGE_SIZE / sizeof(int);  // 这里每个页目录表是1024项
    for(int i = 0;i<count;i++, vaddr += PAGE_SIZE){
        releasePhysicalPages(type, vaddr2paddr(vaddr), 1);

        // 设置页表项为不存在
        pte = (int*)toPTE(vaddr);
        *pte = 0;
    }
    // 释放虚拟页，这一步从用户角度是释放内存的过程，但是从操作系统层面，上一步才是真正的释放内存
    releaseVirtualPages(type, virtualAddress, count);
}

void MemoryManager::releaseVirtualPages(enum AddressPoolType type, const int virtualAddress, const int count){
    if(type == AddressPoolType::KERNEL){
        kernelVirtual.release(virtualAddress, count);
    }
    else if (type == AddressPoolType::KERNEL){
        programManager.running->userVirtual.release(virtualAddress, count);
    }
}

void MemoryManager::releasePhysicalPages(enum AddressPoolType type, const int paddr, const int count)
{
    if (type == AddressPoolType::KERNEL)
    {
        kernelPhysical.release(paddr, count);
    }
    else if (type == AddressPoolType::USER)
    {

        userPhysical.release(paddr, count);
    }
}
int MemoryManager::vaddr2paddr(int vaddr){
    int *pte = (int*)toPTE(vaddr);
    int page = (*pte) & 0xfffff000;
    int offset = vaddr & 0xfff;
    return (page + offset);
}

FCB* MemoryManager::allocateFCB(){
    for(int i = 0;i<MAX_FCB_COUNT;i++){
        if (!fcb_status[i]){
            fcb_status[i] = true;
            return (FCB *)((int)fcb_sets + sizeof(FCB) * i);
        }
    }
}