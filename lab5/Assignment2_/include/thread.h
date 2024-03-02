#ifndef THREAD_H
#define THREAD_H

#include"list.h"
#include "os_constant.h"

typedef void (*ThreadFunction)(void *);

// 线程五个状态
enum ProgramStatus{
    CREATED,
    RUNNING,
    READY,
    BLOCKED,
    DEAD
};

// 进程控制块，这里用来描述线程属性
struct PCB{
    int * stack;    // 栈指针，在进程切换出去时保存栈指针esp，后面可恢复esp
    char name[MAX_PROGRAM_NAME+1];  // 线程名
    enum ProgramStatus status;  // 线程状态
    int priority;   // 线程优先级
    int pid;        // 线程序号
    int ticks;      // 线程时间片总时间
    int ticksPassedBy;  // 线程已执行时间，以实时钟中断为单位
    
    ListNode tagInGeneralList;
    ListNode tagInAllList;
};
#endif