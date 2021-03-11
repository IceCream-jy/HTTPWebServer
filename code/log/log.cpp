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