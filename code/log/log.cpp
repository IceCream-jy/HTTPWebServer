#include "log.hh"

log::log() {
    __lineCount = 0;
    __isAsync = false;
    __writeThread = nullptr;
    __deque = nullptr;
    __today = 0;
    __fp = nullptr;
}

log::~log() {
    if(__writeThread && __writeThread->joinable()) {
        while(!__deque->empty()) {
            __deque->flush();
        };
        __deque->close();
        __writeThread->join();
    }
    if(__fp) {
        std::lock_guard<std::mutex> locker(__mtx);
        flush();
        fclose(__fp);
    }
}

void log::init(int level = 1, const char* path, 
                const char* suffix, int maxQueueSize) {
    __isOpen = true;
    __level = level;
    if(maxQueueSize > 0) {
        __isAsync = true;
        if(!__deque) {
            std::unique_ptr<blockQue<std::string>> newDeque(new blockQue<std::string>);
            __deque = std::move(newDeque);
            
            std::unique_ptr<std::thread> NewThread(new std::thread(flushLogThread));
            __writeThread = move(NewThread);
        }
    } else {
        __isAsync = false;
    }

    __lineCount = 0;

    time_t timer = time(nullptr);
    struct tm* sysTime = localtime(&timer);
    struct tm t = *sysTime;
    __path = path;
    __suffix = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            __path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, __suffix);
    __today = t.tm_mday;

    do{
        std::lock_guard<std::mutex> locker(__mtx);
        __buff.retrieveAll();
        if(__fp) { 
            flush();
            fclose(__fp); 
        }

        __fp = fopen(fileName, "a");
        if(__fp == nullptr) {
            mkdir(__path, 0777);
            __fp = fopen(fileName, "a");
        } 
        assert(__fp != nullptr);
    }while(0);
}

log* log::instance(){
    static log instance;
    return &instance;
}

void log::flushLogThread(){
    log::instance()->__asyncWrite();
}

void log::write(int level, const char *format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    /* 日志日期 日志行数 */
    if (__today != t.tm_mday || (__lineCount && (__lineCount  %  MAX_LINES == 0))){
        std::unique_lock<std::mutex> locker(__mtx);
        locker.unlock();
        
        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if (__today != t.tm_mday){
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", __path, tail, __suffix);
            __today = t.tm_mday;
            __lineCount = 0;
        }
        else {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", __path, tail, (__lineCount  / MAX_LINES), __suffix);
        }
        
        locker.lock();
        flush();
        fclose(__fp);
        __fp = fopen(newFile, "a");
        assert(__fp != nullptr);
    }

    do{
        std::unique_lock<std::mutex> locker(__mtx);
        __lineCount++;
        int n = snprintf(__buff.beginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
                    
        __buff.hasWritten(n);
        __appendLogLevelTitle(level);

        va_start(vaList, format);
        int m = vsnprintf(__buff.beginWrite(), __buff.writableBytes(), format, vaList);
        va_end(vaList);

        __buff.hasWritten(m);
        __buff.append("\n\0", 2);

        if(__isAsync && __deque && !__deque->full()) {
            __deque->push_back(__buff.retrieveAllToStr());
        } 
        else {
            fputs(__buff.peek(), __fp);
        }
        __buff.retrieveAll();
    }while(0);
}

void log::flush() {
    if(__isAsync) { 
        __deque->flush(); 
    }
    fflush(__fp);
}

int log::getLevel() {
    std::lock_guard<std::mutex> locker(__mtx);
    return __level;
}

void log::setLevel(int level) {
    std::lock_guard<std::mutex> locker(__mtx);
    __level = level;
}

void log::__appendLogLevelTitle(int level) {
    switch(level) {
    case 0:
        __buff.append("[debug]: ", 9);
        break;
    case 1:
        __buff.append("[info] : ", 9);
        break;
    case 2:
        __buff.append("[warn] : ", 9);
        break;
    case 3:
        __buff.append("[error]: ", 9);
        break;
    default:
        __buff.append("[info] : ", 9);
        break;
    }
}




void log::__asyncWrite() {
    std::string str = "";
    while(__deque->pop(str)) {
        std::lock_guard<std::mutex> locker(__mtx);
        fputs(str.c_str(), __fp);
    }
}