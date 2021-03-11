#ifndef _BLOCK_QUEUE_H_
#define _BLOCK_QUEUE_H_

#include <mutex>
#include <deque>
#include <condition_variable>
#include <assert.h>

#define BLOCKQUE(ret) \
template <class T>\
ret blockQue<T>::

template <class T>
class blockQue{
public:
    explicit blockQue(size_t maxCapacity = 1000){
        assert(maxCapacity > 0);
        __isClose = false;
    }

    ~blockQue(){

    }

    void clear();

    bool empty();

    bool full();

    void close();

    size_t size();

    size_t capacity();

    T front();

    T back();

    void push_back(const T& item);

    void push_front(const T& item);

    bool pop(T& item);

    bool pop(T& item, int timeout);

    void flush();

private:
    std::deque<T> __deq;
    size_t __capacity;
    std::mutex __mtx;
    bool __isClose;
    std::condition_variable __consumer;
    std::condition_variable __producer;
};

BLOCKQUE(void)clear(){
    std::lock_guard<std::mutex> locker(__mtx);
    __deq.clear();
}

BLOCKQUE(bool)empty(){
    std::lock_guard<std::mutex> locker(__mtx);
    return __deq.empty();
}

BLOCKQUE(bool)full(){
    std::lock_guard<std::mutex> locker(__mtx);
    return __deq.size() >= __capacity;
}

BLOCKQUE(void)close(){
    do{
        std::lock_guard<std::mutex> locker(__mtx);
        __deq.clear();
        __isClose = true;
    }while(0);
    __producer.notify_all();
    __consumer.notify_all();
}

BLOCKQUE(size_t)size(){
    std::lock_guard<std::mutex> locker(__mtx);
    return __deq.size();
}

BLOCKQUE(size_t)capacity(){
    return __capacity;
}

BLOCKQUE(T)front(){
    std::lock_guard<std::mutex> locker(__mtx);
    return __deq.front();
}

BLOCKQUE(T)back(){
    std::lock_guard<std::mutex> locker(__mtx);
    return __deq.back();
}

BLOCKQUE(void)push_back(const T& item){
    std::unique_lock<std::mutex> locker(__mtx);
    while(__deq.size() >= __capacity){
        __producer.wait(locker);
    }
    __deq.push_back(item);
    __consumer.notify_one();
}

BLOCKQUE(void)push_front(const T &item) {
    std::unique_lock<std::mutex> locker(__mtx);
    while(__deq.size() >= __capacity) {
        __producer.wait(locker);
    }
    __deq.push_front(item);
    __consumer.notify_one();
}

BLOCKQUE(bool)pop(T& item){
    std::unique_lock<std::mutex> locker(__mtx);
    while(__deq.empty()){
        __consumer.wait(locker);
        if(__isClose){
            return false;
        }
    }
    item = __deq.front();
    __deq.pop_front();
    __producer.notify_one();
    return true;
}

BLOCKQUE(bool)pop(T& item, int timeout){
    std::unique_lock<std::mutex> locker(__mtx);
    while(__deq.empty()){
        if(__consumer.wait_for(locker, 
                std::chrono::seconds(timeout))
                == std::cv_status::timeout){
            return false;
        }
        if(__isClose)return false;
        item = __deq.front();
        __deq.pop_front();
        __producer.notify_one();
        return true;
    }
}

BLOCKQUE(void)flush(){
    __consumer.notify_one();
}

#endif