#include "EventLoop.h"
#include "Acceptor.h"
#include "utils/Socket.h"
#include "utils/InetAddress.h"
#include "utils/SocketsOps.h"
#include "libnet/base/logger.h"

using namespace libnet;


Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),acceptSocket_(sockets::createNonblockingOrDie()),
  acceptChannel_(loop, acceptSocket_.fd()),listenning_(false)
{
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(
      std::bind(&Acceptor::handleRead, this)
      );
  
}

void Acceptor::listen()
{
  loop_->assertInLoopThread();
  listenning_=true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
  loop_->assertInLoopThread();
  InetAddress peerAddr(0);
  int connfd = acceptSocket_.accept(&peerAddr);
  if( connfd > 0)
  {
    if( nccb_)
    {
      nccb_(connfd, peerAddr);
      LOG_DEBUG<<"acceptor handle new connection\n";
    }
    else
    {
      sockets::close(connfd);
      LOG_DEBUG<<"acceptor close new connection\n";
    } 
  }
}