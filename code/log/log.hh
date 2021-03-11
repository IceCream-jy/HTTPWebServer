#ifndef _LOG_H_
#define _LOG_H_

#include <deque>
#include <mutex>
#include <thread>

#include "blockqueue.hh"
#include "../buffer/buffer.hh"

class log{
public:
    void init(int level, const char* path = "./log", 
                const char* suffix =".log",
                int maxQueueCapacity = 1024);

    static log* instance();
    static void flushLogThread();

    void write(int level, const char *format,...);
    void flush();

    int getLevel();
    void setLevel(int level);
    bool isOpen() { return __isOpen; }
private:
    log();
    void appendLogLevelTitle_(int level);
    virtual ~log();
    void asyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* __path;     // 路径
    const char* __suffix;

    int __MAX_LINES;

    int __lineCount;    // 日至行数记录
    int __today;        // 记录当前时间是哪一天

    bool __isOpen;
 
    buffer __buff;
    int __level;
    bool __isAsync;     // 是否异步

    FILE* __fp;         // 打开 log 的文件指针
    std::unique_ptr<blockQue<std::string>> __deque; // 指向阻塞队列
    std::unique_ptr<std::thread> __writeThread;     // 指向写日志线程
    std::mutex __mtx;
};

#endif