#ifndef SYNC_H
#define SYNC_H

#include "os_type.h"
#include "asm_utils.h"
#include "list.h"
#include "os_modules.h"

// 自旋锁
class SpinLock{
private:
    uint32 bolt;    // 共享变量锁，0代表没上锁
public:
    SpinLock();
    void initialize();
    void lock();    // 上锁
    void unlock();  // 解锁
};

// 信号量
class Semaphore{
private:
    uint32 counter;
    List waiting;   // 等待队列，其中的线程等待信号量执行V操作
    SpinLock sem_lock;  // 锁住counter共享变量

public:
    Semaphore();
    void initialize(uint32);
    void P();
    void V();
};

// 互斥锁
class Mutex{
private:
    bool available;
public:
    Mutex();
    void initialize();
    void lock();
    void unlock();
};
#endif