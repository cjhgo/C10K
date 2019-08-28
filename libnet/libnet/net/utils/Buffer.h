#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

namespace libnet
{

class Buffer
{

 public:
  Buffer()
    : buffer_(kCheapPrepend+kInitialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
  {

  }
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;

  ssize_t readFd(int fd, int* savedError);
  void append(const std::string& str);
  void append(const char* data, size_t len);
  std::string retriveAsString();
  

  size_t writableBytes()
  {
    return buffer_.size() - writerIndex_;
  }
  size_t readableBytes()
  {
    return writerIndex_ - readerIndex_;
  }

  size_t prependableBytes()
  {
    return readerIndex_;
  }

  void ensureWritableBytes(size_t len)
  {
    if(writableBytes() < len)
    makeSpace(len);
  }

  void retrieve(size_t len)
  {
    readerIndex_ += len;
  }

  void retrieveAll()
  {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }
  char* peek()
  {
    return begin()+readerIndex_;
  }
private:
  char * begin()
  {
    return &*buffer_.begin();
  }

  void makeSpace(size_t len)
  {
    if(prependableBytes() + writableBytes() >= ( kCheapPrepend + len) )
    {
      size_t readable = readableBytes();
      std::copy(begin()+readerIndex_, begin()+writerIndex_, begin()+kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_+readable;
      assert(readable == readableBytes());
    }else
    {
      buffer_.resize(writerIndex_+len);
    }
  }

 private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};

}

#endif