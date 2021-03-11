#include "sqlconnpool.hh"

sqlConnPool::sqlConnPool(){
    __useCount = __freeCount = 0;
}
sqlConnPool::~sqlConnPool(){
    closePool();
}

sqlConnPool* sqlConnPool::instance(){
    static sqlConnPool instance;
    return &instance;
}

void sqlConnPool::init(const char* host, int port,
                       const char* user,const char* pwd, 
                       const char* dbName, int maxConnSize = 10) {
    assert(maxConnSize > 0);
    for (int i = 0; i < maxConnSize; i++) {
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if (!sql) {
            LOG_ERROR("MySql init error!"); 
            assert(sql);
        }
        sql = mysql_real_connect(sql, host,
                                 user, pwd,
                                 dbName, port, 
                                 nullptr, 0);
        if (!sql) {
            LOG_ERROR("MySql Connect error!"); 
        }
        __connQue.push(sql);
    }
    __MAX_CONN = maxConnSize;
    sem_init(&__semId, 0, __MAX_CONN);
}

MYSQL* sqlConnPool::getConn() {
    MYSQL *sql = nullptr;
    if(__connQue.empty()){
        LOG_WARN("SqlConnPool busy!"); 
        return nullptr;
    }
    sem_wait(&__semId);
    do{
        std::lock_guard<std::mutex> locker(__mtx);
        sql = __connQue.front();
        __connQue.pop();
    }while(0);
    return sql;
}

void sqlConnPool::freeConn(MYSQL* conn){
    assert(conn);
    std::lock_guard<std::mutex> locker(__mtx);
    __connQue.push(conn);
    sem_post(&__semId);
}

int sqlConnPool::getFreeConnCount() {
    std::lock_guard<std::mutex> locker(__mtx);
    return __connQue.size();
}

void sqlConnPool::closePool(){
    std::lock_guard<std::mutex> locker(__mtx);
    while(!__connQue.empty()){
        auto item = __connQue.front();
        __connQue.pop();
        mysql_close(item);
    }
    mysql_library_end();  
}