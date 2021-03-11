#ifndef _LOG_H_
#define _LOG_H_

#include <deque>
#include <mutex>
#include <thread>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>

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
    void __appendLogLevelTitle(int level);
    virtual ~log();
    void __asyncWrite();                        // 将 __deque 中所有字符串写入 __fp

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* __path;     // 路径
    const char* __suffix;   // 后缀

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

#define LOG_BASE(level, format, ...) \
    do {\
        log* m_log = log::instance();\
        if (m_log->isOpen() && m_log->getLevel() <= level) {\
            m_log->write(level, format, ##__VA_ARGS__); \
            m_log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);


#endif