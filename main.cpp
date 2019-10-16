/*************************************************************************
	> File Name: main.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 16 Oct 2019 05:14:07 PM CST
	> Target: 
 ************************************************************************/

#include <iostream>
#include "ThreadPool.h"
using namespace std;

int num = 1;

void* func(void *arg) {
    num++;
}

int main() {
    ThreadPool *pool = new ThreadPool(10, 10);
    TaskFunc fun = static_cast<TaskFunc>(func);
    std::shared_ptr<ThreadTask> threadTask(new ThreadTask);
    threadTask->taskFunc_ = fun;
    threadTask->taskArgs_ = static_cast<void*>(nullptr);
    pool->Run();
    num = 1;
    while(true) {
        sleep(1);
        cout << num << endl;
        pool->TaskAdd(threadTask);   
    }

    return 0;
}
