#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include <memory>
#include "Callbacks.h"
#include "utils/InetAddress.h"
namespace libnet
{


class Channel;
class EventLoop;
class Socket;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
class TcpConnection:public std::enable_shared_from_this<TcpConnection>
{

 public:
  TcpConnection(EventLoop* loop, const std::string& name,  int  sockfd, 
            const InetAddress& localAddr, const InetAddress& peerAddr);
          
  ~TcpConnection();

  EventLoop* getLoop() const { return loop_;}

  const std::string& name() const { return name_; }

  const InetAddress& localAddress() { return localAddr_;}
  const InetAddress& pererAddress() { return peerAddr_;}

  bool connected() const { return state_ == kConnected;}


  void setMessageCallback(const MessageCallback& cb){ messagecb_ = cb;}
  void setConnectionCallback(const ConnectionCallback& cb){ concb_ = cb;}
  
  void setCloseCallback(const CloseCallback& cb){ closecb_ = cb;}
  void connectEstablished();
  void connectDestroyed();
 private:
  enum StateE { kConnecting, kConnected, kDisconnected,};

  void setState(StateE s) { state_ = s;}
  void handleRead();  
  void handleWrite();
  void handleClose();
  void handleError();

  EventLoop* loop_;
  std::string name_;
  

  
  StateE state_;
  std::shared_ptr<Channel> channel_;
  std::shared_ptr<Socket> socket_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  ConnectionCallback concb_;//及处理连接建立也处理连接断开
  MessageCallback messagecb_;
  CloseCallback closecb_;
};


}

#endif