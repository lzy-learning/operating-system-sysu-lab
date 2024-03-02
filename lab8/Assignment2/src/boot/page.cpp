#include "os_constant.h"

extern "C" void open_page_mechanism(){
    /*
    页目录表和页表都是1024大小，表项也都是4B
    */
    // 页目录表首地址：0x100000
    int *directory = (int*)PAGE_DIRECTORY;
    // 页表首地址
    int *page = (int*)(PAGE_DIRECTORY+PAGE_SIZE);
    int amount = PAGE_SIZE/4;

    
    for(int i = 0;i<amount;i++){
        directory[i] = 0;   // 初始化页目录表
        page[i] = 0;        // 初始化页表
    }

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

    
    directory[0] = ((int)page)|0x07;
    // 3GB的内核空间
    directory[768] = directory[0];
    // 最后一个页目录项指向页目录表，这里是为了在开启二级分页机制后程序员能够方便主动设置页目录表和页表
    directory[1023] = ((int)directory) | 0x7;

    // // 初始化cr3，cr0，开启分页机制
    // asm_init_page_reg(directory);

    // printf("open page mechanism\n");

}