#include "buffer.h"

Buffer::Buffer(int bufferSize):buffer(bufferSize), readPos(0), writePos(0){}

void Buffer::initPtr()
{
    bzero(&buffer[0], buffer.size());
    readPos = 0;
    writePos = 0;
}

size_t Buffer::readBytes() const
{
    return readPos;
}

size_t Buffer::readableByte() const
{
    return writePos - readPos;
}

size_t Buffer::writeableByte() const
{
    return buffer.size() - writePos;
}

const char* Buffer::curReadPtr() const
{
    return beginPtr() + readPos;
}

const char* Buffer::curWritePtrConst() const
{
    return beginPtr() + writePos;
}

char* Buffer::curWritePtr()
{
    return beginPtr() + writePos;
}

void Buffer::updateReadPtr(size_t len)
{
    assert(len <= readableByte());
    readPos += len;
}

void Buffer::updateReadPtrToEnd(const char* end)
{
    assert(end >= curReadPtr());
    updateReadPtr(end - curReadPtr());
}

void Buffer::updateWritePtr(size_t len)
{
    assert(len <= writeableByte());
    writePos += len;
}

void Buffer::allocateSpace(size_t len)
{
    if(writeableByte() + readBytes() < len){
        buffer.resize(writePos + len + 1);
    }
    else{
        size_t readable = readableByte();
        std::copy(beginPtr() + readPos, beginPtr() + writePos, beginPtr());
        readPos = 0;
        writePos = readable;
        assert(readable == readableByte());
    }
}

void Buffer::ensureWrite(size_t len)
{
    if(writeableByte() < len)
        allocateSpace(len);
    assert(writeableByte() >= len);
}

void Buffer::append(const char* str, size_t len)
{
    assert(str);
    ensureWrite(len);
    std::copy(str, str + len, curWritePtr());
    updateWritePtr(len);
}

void Buffer::append(const std::string& str)
{
    append(str.data(), str.length());
}

void Buffer::append(const void* data, size_t len)
{
    assert(data);
    append(static_cast<const char*>(data), len);
}

void Buffer::append(const Buffer& buffer)
{
    append(buffer.curReadPtr(), buffer.readableByte());
}

ssize_t Buffer::readFd(int fd, int* Errno)
{
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = writeableByte();

    iov[0].iov_base = beginPtr() + writePos;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0){
        *Errno = errno;
    }
    else if(static_cast<size_t>(len) <= writeable){
        writePos += len;
    }
    else{
        writePos = buffer.size();
        append(buff, len - writeable);
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int* Errno)
{
    size_t readSize = readableByte();
    ssize_t len = write(fd, curReadPtr(), readSize);
    if(len < 0){
        *Errno = errno;
        return len;
    }
    readPos += len;
    return len;
}

std::string Buffer::bufferToStr()
{
    std::string str(curReadPtr(), readableByte());
    initPtr();
    return str;
}

char* Buffer::beginPtr()
{
    return &*buffer.begin();
}

const char* Buffer::beginPtr() const
{
    return &*buffer.begin();
}