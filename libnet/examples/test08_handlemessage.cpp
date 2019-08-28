#include "libnet/libnet.hpp"


using namespace libnet;


void onConnection(const TcpConnectionPtr& conn)
{
  if( conn->connected())
  {
    LOG_DEBUG<<"onconnection(): new connection "<<conn->name().c_str()<<
    "from"<<conn->pererAddress().toHostPort().c_str()<<"\n";

  }else
  {
    LOG_DEBUG<<"onconnection(): connection "<<conn->name().c_str()<<
    "is down\n";
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receivetime)
{
  size_t x = buf->readableBytes();
  LOG_DEBUG<<"on message(): recevie "<<x<<"bytes :"<<buf->retriveAsString().c_str()<<" from "<<conn->name().c_str()<<"\n"
      <<"at "<<receivetime.toFormattedString().c_str()<<"\n";
}


int main(int argc, char const *argv[])
{
  printf("main(): begin\n");
  // Logger::instance().setLogLevel(NONE);
  EventLoop loop;
  InetAddress listenAddr(9988);
  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  
  server.start();
  loop.loop();
  return 0;
}
