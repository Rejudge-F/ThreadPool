/*************************************************************************
	> File Name: ThreadPool.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 16 Oct 2019 04:03:05 PM CST
	> Target: 
 ************************************************************************/

#include "ThreadPool.h"

const int time_wait = 5;

void* StartAdjust(void *arg) {
    ThreadPool *pool = static_cast<ThreadPool *>(arg);
    pool->AdjustThread();
}


ThreadPool::ThreadPool(unsigned int minThreadNum, unsigned int maxThreadNum):
    shutDown_(false),
    livThreadNum_(0),
    minThreadNum_(minThreadNum),
    maxThreadNum_(maxThreadNum),
    taskSize_(0),
    busyThreadNum_(0)
{
    pthread_mutex_init(&mutex_, nullptr);
    pthread_mutex_init(&taskMutex_, nullptr);
    pthread_cond_init(&cond_, nullptr);
    pthread_mutex_init(&counterMutex_, nullptr);
}

ThreadPool::~ThreadPool() {
    if(taskSize_ > 0) {
        for(int i = 0; i < time_wait; i++) {
            sleep(1);
            std::cout << "taskSize: " << TaskSize() << ", " << time_wait - i << "s will shut!\n";
        }
    }
    Destroy();
    pthread_cond_destroy(&cond_);  
    pthread_mutex_lock(&mutex_);
    pthread_mutex_destroy(&mutex_);
    while(!taskQueue_.empty()) {
        taskQueue_.pop();
    }
    while(!threadVector_.empty()) {
        threadVector_.pop_back();
    }
}

unsigned int ThreadPool::TaskSize() {
    return taskSize_;
}

std::shared_ptr<ThreadTask> ThreadPool::GetThreadTask() {
    pthread_mutex_lock(&taskMutex_);
    if(!taskQueue_.empty()) {
        auto task = taskQueue_.front();
        taskQueue_.pop();
        taskSize_--;
        pthread_mutex_unlock(&taskMutex_);
        return task;
    }
    pthread_mutex_unlock(&taskMutex_);
    return nullptr;
}

void ThreadPool::IncBusy() {
    pthread_mutex_lock(&counterMutex_);
    busyThreadNum_++;
    pthread_mutex_unlock(&counterMutex_);
}

void ThreadPool::DscBusy() {
    pthread_mutex_lock(&counterMutex_);
    busyThreadNum_--;
    pthread_mutex_unlock(&counterMutex_);
}

void ThreadPool::TaskAdd(std::shared_ptr<ThreadTask> threadTask) {
    pthread_mutex_lock(&taskMutex_);
    taskQueue_.push(threadTask);
    taskSize_++;
    pthread_mutex_unlock(&taskMutex_);
    pthread_cond_broadcast(&cond_);
}

void ThreadPool::Destroy() {
    for(auto thread : threadVector_) {
        thread->Shut();
    } 
}

void ThreadPool::Run() {
    for(int i = 0; i < minThreadNum_; i++) {
        std::shared_ptr<Thread> thread(new Thread(&cond_, &mutex_, std::shared_ptr<ThreadPool>(this)));
        thread->Start();
        threadVector_.push_back(thread);
        livThreadNum_++;
    }
    pthread_t pid2;
    pthread_create(&pid2, nullptr, StartAdjust, static_cast<void*>(this));

}

void ThreadPool::AdjustThread() {
    while(true) {
        //std::cout << busyThreadNum_ << " vs " << livThreadNum_ << std::endl;
        if(taskSize_ > livThreadNum_ / 2) {
            for(int i = 0; i < minThreadNum_ && livThreadNum_ < maxThreadNum_; i++) {
                std::shared_ptr<Thread> thread(new Thread(&cond_, &mutex_, std::shared_ptr<ThreadPool>(this)));
                thread->Start();
                threadVector_.push_back(thread);
                livThreadNum_++;
            }
        } else {
            for(int i = 0; i < livThreadNum_ - taskSize_ * 2 && livThreadNum_ > minThreadNum_; i++) {
                if(threadVector_.at(0)->IsAlive()) {
                    threadVector_.at(0)->Shut();
                    livThreadNum_--;
                    threadVector_.pop_front();
                } else {
                    threadVector_.pop_front();
                    livThreadNum_--;
                }
            }
        }
    }
}
