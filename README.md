# HTTPWebServer

A HTTP web server with c++11

Linux下的高性能HTTP web服务器，使用 C++11 实现，经过 webbench 压力测试可以通过上万的QPS

## 环境配置

- Ubuntu 20.04.2
- gcc version 9.3.0
- mysql Ver 8.0.23

## 功能

- 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
- 利用正则与状态机解析HTTP请求报文，实现处理静态资源的请求；
- 利用标准库容器封装char，实现自动增长的缓冲区；
- 基于小根堆实现的定时器，关闭超时的非活动连接；
- 利用单例模式与阻塞队列实现异步的日志系统，记录服务器运行状态；
- 利用RAII机制实现了数据库连接池，减少数据库连接建立与关闭的开销，同时实现了用户注册登录功能。

