/*************************************************************************
	> File Name: Thread.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 16 Oct 2019 12:51:29 PM CST
	> Target: 
 ************************************************************************/

#include "Thread.h"
#include "ThreadPool.h"
using namespace std;

struct ThreadData {
    bool *running_;
    bool *shutDown_;
    std::shared_ptr<ThreadPool> threadHolder_;
    pthread_cond_t *cond_;
    pthread_mutex_t *mutex_;
    std::shared_ptr<ThreadTask> threadTask_;

    ThreadData(bool *running, bool *shutDown, std::shared_ptr<ThreadPool> threadHolder, pthread_cond_t *cond, pthread_mutex_t *mutex) :
    running_(running),
    shutDown_(shutDown),
    threadHolder_(threadHolder),
    cond_(cond),
    mutex_(mutex),
    threadTask_(nullptr)
    {
    }
};

void *callBack(void *args) {
    ThreadData *threadData = static_cast<ThreadData*>(args);
    while(true) {
#if DEBUG
        std::cout << pthread_self() << " ready\n";
#endif
        
        while(*threadData->running_ && threadData->threadHolder_->TaskSize() == 0) {
            pthread_cond_wait(threadData->cond_, threadData->mutex_);
        }

        if(*threadData->shutDown_) { 
            std::cout << "pthread exit\n";
            break;
        }
       
        threadData->threadTask_ = threadData->threadHolder_->GetThreadTask();
        if(threadData->threadTask_ != nullptr) {
            threadData->threadHolder_->IncBusy();
            threadData->threadTask_->taskFunc_(threadData->threadTask_->taskArgs_);
            threadData->threadHolder_->DscBusy();
        }
        threadData->threadTask_ = nullptr;
    }
}

Thread::Thread(pthread_cond_t *cond, pthread_mutex_t *mutex, std::shared_ptr<ThreadPool> threadHolder) :
    cond_(cond),
    mutex_(mutex),
    threadHolder_(threadHolder),
    running_(false),
    shutDown_(false)
{
}

Thread::~Thread() {
    cond_ = nullptr;
}

void Thread::Start() {
    ThreadData *threadData = new ThreadData(&running_, &shutDown_, threadHolder_, cond_, mutex_);
    if(pthread_create(&pid_, nullptr, callBack, threadData)) {
        std::cerr << __FILE__ << __LINE__ << ": create thread falied" << std::endl;
        exit(1);
    }
    running_ = true;
}

void Thread::Shut() {
    shutDown_ = true;
    std::cout << "thread_exit\n";
}


