#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <string>
#include <memory>
#include <map>
#include "Callbacks.h"
#include "TcpConnection.h"

namespace libnet
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
 public:
  TcpServer(EventLoop* loop, const InetAddress& listenAddr);
  ~TcpServer();

  void start();
  void setMessageCallback(const MessageCallback& cb){ messagecb_ = cb;}
  void setConnectionCallback(const ConnectionCallback& cb){ conncb_ = cb;}
  void setWriteCompleteCallback(const WriteCompleteCallback&cb){ wccb_ = cb; }
  
  void setThreadNum(int numThreads);
 private:
  //处理新连接的建立,由acceptor.handleRead调用
  void newConnection(int sockfd, const InetAddress& peerAddr);

  //从map中去除已经关闭的连接
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  EventLoop* loop_;
  const std::string name_;  

  std::shared_ptr<Acceptor> acceptor_;
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  ConnectionCallback conncb_;
  MessageCallback messagecb_;
  WriteCompleteCallback wccb_;
  bool started_;
  int nextConnId_;
  ConnectionMap connections_;
};


}
#endif