#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include "utils/Socket.h"
#include "Channel.h"
namespace libnet
{
class EventLoop;
class InetAddress;


class Acceptor
{
 public:

  Acceptor(EventLoop* loop, const InetAddress& listenAddr);

  typedef std::function< void(int sockfd, const InetAddress&)>
            NewConnectionCallback;

  void setNewConnectionCallback(const NewConnectionCallback cb)
  {nccb_ =cb;}

  bool listening() const { return listenning_;}
  void listen();
 private:
  void handleRead();
  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback nccb_;
  bool listenning_;
};
}
#endif