#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "utils/Socket.h"
#include <unistd.h>

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

void TcpConnection::handleRead()
{
  char buf[65536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
  messagecb_(shared_from_this(),buf, n);
}