#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <cstring>
#include <string>
#include <vector>
#include <assert.h>
#include <atomic>
#include <unistd.h>
#include <sys/uio.h>

class buffer {
public:
    buffer(int initBuffSize = 1024);
    ~buffer() = default;
    // 可写字节数
    size_t writableBytes() const; 
    // 可读字节数        
    size_t readableBytes() const ;    
    // 前置字节数，即已读字节数  
    size_t prependableBytes() const;    
    // 返回读字符串位置
    const char* peek() const;    
    // 保证有 len 长度的可写距离       
    void ensureWriteable(size_t len);   
    // 将 __writePos 向后移动 len 长度
    void hasWritten(size_t len);   
     // 移动 __readPos ，表示又读了 len 长度     
    void retrieve(size_t len);         
    // 将 __readPos 移动至 end 位置
    void retrieveUntil(const char* end);
    // 将读写 Pos 全部置为0，__buffer 全部清零
    void retrieveAll() ;                
    // 返回未读字符串，并全部清零
    std::string retrieveAllToStr();     
    // 返回可写位置
    const char* beginWriteConst() const;        
    char* beginWrite();
    // 将 str 拼接到未读后面，如果空间不够将扩容
    void append(const std::string& str);        
    void append(const char* str, size_t len);
    void append(const void* data, size_t len);
    void append(const buffer& buff);
    // 如果空间不够，将散列读入到一个 buff 数组，再调用 append 拼接
    ssize_t readFd(int fd, int* Errno);    
    // 将可读内容写入 fd 
    ssize_t writeFd(int fd, int* Errno);    

private:
    char* __beginPtr();
    const char* __beginPtr() const;
    // 如果空间够，将未读前置到开始，否则重新申请空间
    void __makeSpace(size_t len);           

    std::vector<char> __buffer;
    std::atomic<std::size_t> __readPos;
    std::atomic<std::size_t> __writePos;
};


#endif