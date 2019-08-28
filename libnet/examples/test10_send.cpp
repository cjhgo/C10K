#include "libnet/libnet.hpp"
#include <algorithm>

using namespace libnet;

std::string message1,message2;

void onConnection(const TcpConnectionPtr& conn)
{
  if(conn->connected())
  {
    printf("onconnection: new connection %s from %s\n", conn->name().c_str(), conn->pererAddress().toHostPort().c_str());
    conn->send(message1);
    conn->send(message2);
    conn->shutdown();
  }else
  { 
    printf("....!!\n");
    conn->send("fjsdkfdsdsf");
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
  buf->retrieveAll();
}

int main(int argc, char const *argv[])
{
  int len1 = 100;
  int len2 = 200;

  message1.resize(len1);
  message2.resize(len2);

  std::fill(message1.begin(),message1.end(),'A');
  std::fill(message2.begin(),message2.end(),'B');

  InetAddress listenAddr(9989);
  EventLoop loop;
  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();
  loop.loop();

  return 0;
}
