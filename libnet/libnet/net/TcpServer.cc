#include <functional>
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "utils/SocketsOps.h"

#include <assert.h>

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
  if(!acceptor_->listening())
  {
    loop_->runInLoop(
      std::bind(&Acceptor::listen, acceptor_)
    );
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
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  conn->connectEstablished();

}



void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  size_t n = connections_.erase(conn->name());
  assert(n == 1);
  loop_->queueInLoop(
    std::bind(&TcpConnection::connectDestroyed, conn)
  );
}