#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "utils/Socket.h"
#include "libnet/base/logger.h"

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
    std::bind(&TcpConnection::handleRead, this, std::placeholders::_1)
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


void TcpConnection::send(const std::string& message)
{
  if( state_ == kConnected)
  {
    if(loop_->isInLoopThread())
    {
      sendInLoop(message);
    }else
    {
      loop_->runInLoop(
        std::bind(&TcpConnection::sendInLoop, this, message)
      );
    }
    
  }
}

void TcpConnection::sendInLoop(const std::string& message)
{
  loop_->assertInLoopThread();
  ssize_t nwrote  = 0;

  if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
  {
    nwrote = ::write(channel_->fd(), message.data(), message.size());
    if( nwrote >= 0)
    {
      if( nwrote < message.size())
      {

      }
      else if(wccb_)
      {
        loop_->queueInLoop(
          std::bind(wccb_, shared_from_this())
        );
      }
    }else if(nwrote < 0)
    {

    }
  }
  assert(nwrote >= 0);
  if(nwrote < message.size())
  {
    outputBuffer_.append(message.data()+nwrote, message.size()-nwrote);
    if(!channel_->isWriting())
    {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::shutdown()
{
  if(state_ == kConnected)
  {
    setState(kDisConnecting);
    loop_->runInLoop(
      std::bind(&TcpConnection::shutdownInLoop, this)
    );
  }

}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  if(!channel_->isWriting())
  {
    socket_->shutdownWrite();
  }
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
  assert(state_ == kConnected || state_ == kDisConnecting);
  setState(kDisconnected);
  channel_->disableAll();
  concb_(shared_from_this());

  loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead(Timestamp receiveTime)
{
  int error=0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &error);    
  if( n > 0)
  {
    messagecb_(shared_from_this(),&inputBuffer_,receiveTime);
  }else if( n == 0)
  {
    LOG_DEBUG<<"handle close event on read..\n";
    handleClose();
  }else
  {
    handleError();
  }
  
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected || state_ == kDisConnecting);
  channel_->disableAll();
  LOG_DEBUG<<"handle close event on handleclose..\n";
  closecb_(shared_from_this());

  //loop_->
}

void TcpConnection::handleError()
{

}

void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if(channel_->isWriting())
  {
    ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if( n > 0)
    {
      outputBuffer_.retrieve(n);
      if(outputBuffer_.readableBytes() == 0)
      {
        channel_->disableWriting();
        if(wccb_)
        {
          loop_->queueInLoop(
            std::bind(wccb_, shared_from_this())
          );          
        }
        if(state_ == kDisConnecting)
        {
          shutdownInLoop();
        }        
      }else
      {
        LOG_DEBUG<<" need next write...\n";
      }
    }else 
    {

    }
  }else
  {

  }
}