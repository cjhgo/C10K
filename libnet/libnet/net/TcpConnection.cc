#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "utils/Socket.h"
#include <unistd.h>
#include <assert.h>

using namespace libnet;

TcpConnection::TcpConnection(EventLoop* loop, 
                            const std::string& name,  
                            int  sockfd, 
                            const InetAddress& localAddr, 
                            const InetAddress& peerAddr)
    : loop_(loop),name_(name),state_(kConnected),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop,sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr)
{
  channel_->setReadCallback(
    std::bind(&TcpConnection::handleRead, this)
  );


  channel_->setWriteCallback(
    std::bind(&TcpConnection::handleWrite, this)
  );

  channel_->setErrorCallback(
    std::bind(&TcpConnection::handleError, this)
  );

  channel_->setCloseCallback(
    std::bind(&TcpConnection::handleClose, this)
  );

}

TcpConnection::~TcpConnection()
{
}


void TcpConnection::connectEstablished()
{
  //assert...
  setState(kConnected);
  channel_->enableReading();

  concb_(shared_from_this());//??
}


void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();
  concb_(shared_from_this());

  loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead()
{
  char buf[65536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
  if( n > 0)
  {
    messagecb_(shared_from_this(),buf, n);
  }else if( n == 0)
  {
    handleClose();
  }else
  {
    handleError();
  }
  
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();
  concb_(shared_from_this());

  //loop_->
}

void TcpConnection::handleError()
{

}

void TcpConnection::handleWrite()
{

}