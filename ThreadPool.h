/*************************************************************************
	> File Name: ThreadPool.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 16 Oct 2019 03:40:44 PM CST
	> Target: 
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include "Thread.h"
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <deque>
#include <unistd.h>

class ThreadPool{
public:
    ThreadPool(unsigned int minThreadNum, unsigned int maxThreadNum);
    ~ThreadPool();
    unsigned int TaskSize();
    unsigned int GetBusy(){return busyThreadNum_;}
    std::shared_ptr<ThreadTask> GetThreadTask();
    void Run();
    void Shut() {shutDown_ = true;}
    void IncBusy();
    void DscBusy();
    void Destroy();
    void AdjustThread();
    void TaskAdd(std::shared_ptr<ThreadTask> threadTask);
    unsigned int GetLivThreadNum() {return livThreadNum_;}
private:
    bool shutDown_;
    unsigned int livThreadNum_;
    unsigned int minThreadNum_;
    unsigned int maxThreadNum_;
    unsigned int taskSize_;
    unsigned int busyThreadNum_;
    pthread_cond_t cond_;
    pthread_mutex_t mutex_;
    pthread_mutex_t counterMutex_;
    pthread_mutex_t taskMutex_;
    std::queue<std::shared_ptr<ThreadTask>> taskQueue_;
    std::deque<std::shared_ptr<Thread>>    threadVector_;

};

#endif
