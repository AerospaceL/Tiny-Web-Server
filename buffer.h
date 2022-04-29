#ifndef BUFFER_H
#define BUFFER_H

#include<iostream>
#include<cstring>
#include<atomic>
#include<vector>
#include<unistd.h> 
#include<sys/uio.h>
#include<assert.h>

class Buffer{
private:
    std::vector<char> buffer; // buffer实体
    std::atomic<std::size_t> readPos; // 读指针位置
    std::atomic<std::size_t> writePos; // 写指针位置
    // 返回buffer初始位置的指针
    char* beginPtr();
    const char* beginPtr() const;
    // 用于缓存区扩容
    void allocateSpace(size_t len);

public:
    Buffer(int bufferSize = 1024);
    ~Buffer();

    void initPtr(); // 初始化读指针和写指针

    size_t readBytes() const; // 缓存区已经读取的字节数
    size_t readableByte() const; // 缓存区可以读取的字节数
    size_t writeableByte() const; // 缓存区可以写入的字节数

    const char* curReadPtr() const; // 获取当前读指针
    const char* curWritePtr() const; // 获取当前写指针
    char* curWritePtr();

    void updateReadPtr(size_t len); // 更新读指针
    void updateReadPtrToEnd(const char* end); // 更新读指针至指定位置
    void updateWritePtr(size_t len); // 更新写指针

    // 向缓存区写入数据
    void append(const char* str, size_t len);
    void append(const std::string& str);
    void append(const void* data, size_t len);
    void append(const Buffer& buffer);
    // 确保数据可以全部写入缓存区
    void ensureWrite(size_t len);

    // IO操作的读写接口
    ssize_t readFd(int fd, int* Errno);
    ssize_t writeFd(int fd, int* Errno);

    // 将缓存区的数据转化为字符串
    std::string bufferToStr(); 

};

#endif