/*************************************************************************
	> File Name: ThreadPool.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 16 Oct 2019 04:03:05 PM CST
	> Target: 
 ************************************************************************/

#include "ThreadPool.h"

const int time_wait = 5;

ThreadPool::ThreadPool(unsigned int minThreadNum, unsigned int maxThreadNum):
    shutDown_(false),
    livThreadNum_(0),
    minThreadNum_(minThreadNum),
    maxThreadNum_(maxThreadNum),
    taskSize_(0),
    busyThreadNum(0)
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
    busyThreadNum++;
    pthread_mutex_unlock(&counterMutex_);
}

void ThreadPool::DscBusy() {
    pthread_mutex_lock(&counterMutex_);
    busyThreadNum--;
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
}
