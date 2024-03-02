#ifndef PROGRAM_H
#define PROGRAM_H
#include "list.h"
#include "os_constant.h"
#include "thread.h"

// 根据线程队列标识，找到线程在线程队列中的位置
#define ListNode2PCB(ADDRESS, LISTNODE) ((PCB*)((int)(ADDRESS) - (int)&((PCB*)0)->LISTNODE))
class ProgramManager{
public:
    List allPrograms;   // 所有状态的线程队列
    List readyPrograms; // 就绪队列
    PCB* running;   // 正在跑的线程的指针

    ProgramManager();
    void initialize();

    // 分配一个PCB
    PCB* allocatePCB();
    // 归还一个PCB
    void releasePCB(PCB* program);

    // 创建一个线程并放入就绪队列
    // 成功则返回pid
    int executeThread(ThreadFunction, void*, const char*, int);

    // 线程调度函数
    void schedule();
};
void program_exit();
#endif