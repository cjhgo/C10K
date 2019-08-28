#include "libnet/net/utils/Buffer.h"
#include <sys/uio.h>


using namespace libnet;

ssize_t Buffer::readFd(int fd, int* savedError)
{
  const size_t writable = writableBytes();
  char extrabuf[65536];
  struct iovec vec[2];
  vec[0].iov_base =  begin()+writerIndex_;
  vec[0].iov_len = writableBytes();
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  ssize_t n = ::readv(fd, vec, 2);
  if( n < 0)
  {
    *savedError = errno;
    
  }else if( n <= writable)
  {
    writerIndex_ += n;
  }
  else if( n > writable)
  {
    writerIndex_ = buffer_.size();
    append(extrabuf,n-writable);
  }
  return n;
}


void Buffer::append(const std::string& str)
{
  
  append(str.data(), str.size());
}


void Buffer::append(const char* data , size_t len)
{
  ensureWritableBytes(len);
  buffer_.insert(buffer_.begin()+writerIndex_, data, data+len);
  writerIndex_ += len;
}


std::string Buffer::retriveAsString()
{
  std::string str(peek(), readableBytes());
  retrieveAll();
  return str;
}