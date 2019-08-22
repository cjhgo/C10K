#include <functional>
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "utils/SocketsOps.h"

using namespace libnet;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    name_(listenAddr.toHostPort()),
    acceptor_(new Acceptor(loop,listenAddr)),
    started_(false),
    nextConnId_(1)
{
  acceptor_->setNewConnectionCallback(
    std::bind(&TcpServer::newConnection, this, std::placeholders::_1,std::placeholders::_2)
    );
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
  if( !started_)
  {
    started_ = true;
  }
  // if( !acceptor_->listening)
  {
    // loop_->run
  }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  //assert
  char buf[32];
  snprintf(buf, sizeof buf, "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;

  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  TcpConnectionPtr conn(
          new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
  
  connections_[connName] = conn;
  conn->setConnectionCallback(conncb_);
  conn->setMessageCallback(messagecb_);
  conn->connectEstablished();

}

