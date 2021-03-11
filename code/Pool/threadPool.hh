#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <assert.h>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

struct Pool{
    std::mutex mtx;
    std::condition_variable cond;
    bool isClosed;
    std::queue<std::function<void()>> tasks;
};

class ThreadPool{
public:
    explicit ThreadPool(size_t threadCount = 8): __pool(std::make_shared<Pool>()) {
            assert(threadCount > 0);
            for(size_t i = 0; i < threadCount; i++) {
                std::thread([pool = __pool] {
                    std::unique_lock<std::mutex> locker(pool->mtx);
                    while(true) {
                        if(!pool->tasks.empty()) {
                            auto task = std::move(pool->tasks.front());
                            pool->tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        } 
                        else if(pool->isClosed) break;
                        else pool->cond.wait(locker);
                    }
                }).detach();
            }
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool(){
        if(static_cast<bool>(__pool)){
            do{
                std::lock_guard<std::mutex> locker(__pool->mtx);
                __pool->isClosed = true;
            }while(false);
            __pool->cond.notify_all();
        }
    }

    template <class T>
    void addTask(T&& task){
        do{
            std::lock_guard<std::mutex> locker(__pool->mtx);
            __pool->tasks.emplace(std::forward<T>(task));
        }while(false);
        __pool->cond.notify_one();
    }

private:
    std::shared_ptr<Pool> __pool;

};

#endif