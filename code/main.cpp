#include <unistd.h>
#include "server/webserver.hh"
#include <stdio.h>
#include "config/config.hh"

int main(int argc, char** argv) {
    /* 守护进程 后台运行 */
    //daemon(1, 0); 
    printf("start\n");
    fflush(stdout);
    /*
    p : port
    m : trigMode
    s : timeoutMS
    o : optLinger(无参， 指定选项则为开启)
    c : connPoolNum
    t: threadPoolNum
    l : openLog(无参，指定选项为开启)
    e : logLevel
    q : logQueSize
    */
    // WebServer server(
    //     1316, 3, 60000, false,             /* 端口 ET模式 timeoutMs 优雅退出  */
    //     3306, "root", "root", "webserver", /* Mysql配置 */
    //     12, 6, true, 1, 1024);             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    Config config;
    config.parse_arg(argc, argv);
    WebServer server(
        config.port, config.trigMode, config.timeoutMS, config.optLinger,             /* 端口 ET模式 timeoutMs 优雅退出  */
        3306, "root", "root", "webserver", /* Mysql配置 */
        config.connPoolNum, config.threadNum, 
        config.openLog, config.logLevel, config.logQueSize);    /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    server.Start();
} 