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
    cout << num << endl;
    sleep(rand() % 5);
}

int main() {
    ThreadPool *pool = new ThreadPool(100, 1000);
    TaskFunc fun = static_cast<TaskFunc>(func);
    std::shared_ptr<ThreadTask> threadTask(new ThreadTask);
    threadTask->taskFunc_ = fun;
    threadTask->taskArgs_ = static_cast<void*>(nullptr);
    pool->Run();
    num = 1;

    sleep(1);
    for(int i = 0; i < 1000; i++) {
        pool->TaskAdd(threadTask);   
    }


    sleep(10);


    return 0;
}
