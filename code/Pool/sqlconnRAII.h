#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H
#include "sqlconnpool.h"

/* 资源在对象构造初始化 资源在对象析构时释放*/
class SqlConnRAII {
public:
    SqlConnRAII(MYSQL** sql, sqlConnPool* connpool) {
        assert(connpool);
        *sql = connpool->getConn();
        __sql = *sql;
        __connpool = connpool;
    }
    
    ~SqlConnRAII() {
        if(__sql) { 
            __connpool->freeConn(__sql); 
        }
    }
    
private:
    MYSQL* __sql;
    sqlConnPool* __connpool;
};

#endif