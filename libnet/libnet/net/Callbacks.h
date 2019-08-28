#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <functional>
#include <memory>
#include "libnet/base/Timestamp.h"
namespace libnet
{
  class Buffer;
  class TcpConnection;
  typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
  typedef std::function<void()> TimerCallback;
  typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
  //typedef std::function<void (const TcpConnectionPtr&, const char* data, ssize_t len)> MessageCallback;
  typedef std::function<void (const TcpConnectionPtr&, Buffer* buf, Timestamp)> MessageCallback;
  typedef std::function<void()> TimerCallback;
  typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
  typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
}

#endif