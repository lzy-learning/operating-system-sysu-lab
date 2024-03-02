# include "sync.h"

SpinLock::SpinLock(){
    initialize();
}
void SpinLock::initialize(){
    bolt = 0;
}

void SpinLock::lock(){
    uint32 key = 1;
    // 当bolt为0时交换之后bolt变为1，然后循环终止，成功上锁
    do {
        asm_atomic_exchange(&key, &bolt);
    }while(key);
}

void SpinLock::unlock(){
    bolt = 0;
}

Semaphore::Semaphore(){
    initialize(0);
}

void Semaphore::initialize(uint32 init_counter){
    counter = init_counter;
    sem_lock.initialize();
    waiting.initialize();
}

void Semaphore::P(){
    PCB *cur = nullptr;

    while(true){
        sem_lock.lock();
        if(counter >0){
            counter--;
            sem_lock.unlock();
            return;
        }

        // 如果信号量的值不大于0则阻塞当前线程
        cur = programManager.running;
        waiting.push_back(&(cur->tagInGeneralList));
        cur->status = ProgramStatus::BLOCKED;

        sem_lock.unlock();
        programManager.schedule();
    }
}

void Semaphore::V(){
    sem_lock.lock();
    counter++;

    // 如果有线程阻塞在这个信号量则将该线程移回就绪队列，采用MESA模式
    if(waiting.size()){
        PCB* thread = ListItem2PCB(waiting.front(), tagInGeneralList);
        waiting.pop_front();
        sem_lock.unlock();
        programManager.MESA_Wakeup(thread);
    }
    else sem_lock.unlock();
}

Mutex::Mutex(){
    this->initialize();
}

void Mutex::initialize(){
    this->available = false;
}

void Mutex::lock(){
    while(asm_test_and_set(&this->available));
}

void Mutex::unlock(){
    this->available = false;
}