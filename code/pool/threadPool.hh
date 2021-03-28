#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <assert.h>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

struct taskQue{
    std::mutex mtx;
    std::condition_variable cond;
    bool isClosed;
    std::queue<std::function<void()>> tasks;
};

class ThreadPool{
public:
    explicit ThreadPool(size_t threadCount = 8): __tasks(std::make_shared<taskQue>()) {
            assert(threadCount > 0);
            for(size_t i = 0; i < threadCount; i++) {
                std::thread([tasks = __tasks] {
                    std::unique_lock<std::mutex> locker(tasks->mtx);
                    while(true) {
                        if(!tasks->tasks.empty()) {
                            auto task = std::move(tasks->tasks.front());
                            tasks->tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        } 
                        else if(tasks->isClosed) break;
                        else tasks->cond.wait(locker);
                    }
                }).detach();
            }
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool(){
        if(static_cast<bool>(__tasks)){
            do{
                std::lock_guard<std::mutex> locker(__tasks->mtx);
                __tasks->isClosed = true;
            }while(false);
            __tasks->cond.notify_all();
        }
    }

    template <class T>
    void addTask(T&& task){
        do{
            std::lock_guard<std::mutex> locker(__tasks->mtx);
            __tasks->tasks.emplace(std::forward<T>(task));
        }while(false);
        __tasks->cond.notify_one();
    }

private:
    std::shared_ptr<taskQue> __tasks;

};

#endif