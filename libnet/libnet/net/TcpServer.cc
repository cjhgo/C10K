#include <functional>
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "utils/SocketsOps.h"

#include <assert.h>

using namespace libnet;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    name_(listenAddr.toHostPort()),
    acceptor_(new Acceptor(loop,listenAddr)),
    threadPool_(new EventLoopThreadPool(loop)),
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

void TcpServer::setThreadNum(int numThreads)
{
  assert(numThreads >= 0);
  threadPool_->setThreadNum(numThreads);
}
void TcpServer::start()
{
  if( !started_)
  {
    started_ = true;
    threadPool_->start();
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
  EventLoop* ioLoop = threadPool_->getNextLoop();
  TcpConnectionPtr conn(
          new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
  
  connections_[connName] = conn;
  conn->setConnectionCallback(conncb_);
  conn->setMessageCallback(messagecb_);
  conn->setWriteCompleteCallback(wccb_);
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}



void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  this->loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
  this->loop_->assertInLoopThread();
  size_t n = connections_.erase(conn->name());
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
    std::bind(&TcpConnection::connectDestroyed, conn)
  );
}