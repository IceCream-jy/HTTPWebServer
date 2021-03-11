#include "buffer.hh"

buffer::buffer(int initBuffSize) : 
            __buffer(initBuffSize), 
            __readPos(0), __writePos(0) {}

size_t buffer::writableBytes() const {
    return __buffer.size() - __writePos;
}

size_t buffer::readableBytes() const {
    return __writePos - __readPos;
}

size_t buffer::prependableBytes() const {
    return __readPos;
}

const char* buffer::peek() const {
    return __beginPtr() + __readPos;
}

void buffer::ensureWriteable(size_t len) {
    if(writableBytes() < len) {
        __makeSpace(len);
    }
    assert(writableBytes() >= len);
}

void buffer::hasWritten(size_t len) {
    __writePos += len;
} 

void buffer::retrieve(size_t len) {
    assert(len <= readableBytes());
    __readPos += len;
}

void buffer::retrieveUntil(const char* end) {
    assert(peek() <= end );
    retrieve(end - peek());
}

void buffer::retrieveAll() {
    bzero(&__buffer[0], __buffer.size());
    __readPos = 0;
    __writePos = 0;
}

std::string buffer::retrieveAllToStr() {
    std::string str(peek(), readableBytes());
    retrieveAll();
    return str;
}

const char* buffer::beginWriteConst() const {
    return __beginPtr() + __writePos;
}

char* buffer::beginWrite() {
    return __beginPtr() + __writePos;
}


void buffer::append(const std::string& str) {
    append(str.data(), str.length());
}

void buffer::append(const void* data, size_t len) {
    assert(data);
    append(static_cast<const char*>(data), len);
}

void buffer::append(const char* str, size_t len) {
    assert(str);
    ensureWriteable(len);
    std::copy(str, str + len, beginWrite());
    hasWritten(len);
}

void buffer::append(const buffer& buff) {
    append(buff.peek(), buff.readableBytes());
}

ssize_t buffer::readFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = writableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = __beginPtr() + __writePos;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        __writePos += len;
    }
    else {
        __writePos = __buffer.size();
        append(buff, len - writable);
    }
    return len;
}

ssize_t buffer::writeFd(int fd, int* saveErrno) {
    size_t readSize = readableBytes();
    ssize_t len = write(fd, peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    __readPos += len;
    return len;
}


char* buffer::__beginPtr() {
    return &*(__buffer.begin());
}

const char* buffer::__beginPtr() const {
    return &*(__buffer.begin());
}

void buffer::__makeSpace(size_t len) {
    if(writableBytes() + prependableBytes() < len) {
        __buffer.resize(__writePos + len + 1);
    } 
    else {
        size_t readable = readableBytes();
        std::copy(__beginPtr() + __readPos, 
                    __beginPtr() + __writePos, 
                    __beginPtr());
        __readPos = 0;
        __writePos = __readPos + readable;
        assert(readable == readableBytes());
    }
}
