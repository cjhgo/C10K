#include "libnet/libnet.hpp"
#include <algorithm>

using namespace libnet;

std::string message1,message2,message;

void onConnection(const TcpConnectionPtr& conn)
{
  if(conn->connected())
  {
    printf("onconnection: new connection %s from %s\n", conn->name().c_str(), conn->pererAddress().toHostPort().c_str());
    conn->send(message);        
  }else
  { 
    printf("....!!\n");
    conn->send("fjsdkfdsdsf");
  }
}

void onWriteCompleteCallback(const TcpConnectionPtr& conn)
{
  conn->send(message2);
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


  std::string line;
  for (int i = 33; i < 127; ++i)
  {
    line.push_back(char(i));
  }
  line += line;

  for (size_t i = 0; i < 127-33; ++i)
  {
    message += line.substr(i, 72) + '\n';
  }
  InetAddress listenAddr(9989);
  EventLoop loop;
  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWriteCompleteCallback(onWriteCompleteCallback);
  server.start();
  loop.loop();

  return 0;
}
