#ifndef _SQLCONN_POOL_H
#define _SQLCONN_POOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <assert.h>
#include "../log/log.hh"

class sqlConnPool {
public:
    static sqlConnPool* instance();     // 单例模式，获取实例

public:
    void init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int maxConnSize);
    MYSQL *getConn();
    void freeConn(MYSQL * conn);
    int getFreeConnCount();  
    void closePool();

private:
    // singleton
    sqlConnPool();
    ~sqlConnPool();

private:
    int __MAX_CONN;
    int __useCount;
    int __freeCount;

    std::queue<MYSQL *> __connQue;
    std::mutex __mtx;
    sem_t __semId;
};


#endif