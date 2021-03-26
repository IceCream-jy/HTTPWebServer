#ifndef _HEAP_TIMER_H_
#define _HEAP_TIMER_H_

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>
#include "../log/log.hh"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};
class HeapTimer {
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { clear(); }
    // 调整指定 id 的结点，expires = now() + newExpires
    void adjust(int id, int newExpires);

    void add(int id, int timeOut, const TimeoutCallBack& cb);
    // 删除指定 id 节点，并触发回调函数
    void doWork(int id);

    void clear();
    // 清除超时节点
    void tick();

    void pop();
    // tick，并返回下一节点 expire 的时间距离
    int GetNextTick();

private:
    // 删除指定位置节点
    void del_(size_t i);
    // 对第 i 个节点执行 sift_up
    void siftup_(size_t i);

    bool siftdown_(size_t index, size_t n);

    void SwapNode_(size_t i, size_t j);

    std::vector<TimerNode> heap_;
    // 保存 id 的个数
    std::unordered_map<int, size_t> ref_;
};

#endif