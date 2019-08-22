#include <cstdio>
#include <iostream>
#include <thread>
#include "libnet/libnet.hpp"


void onConnection(const libnet::TcpConnectionPtr& conn)
{
  if( conn->connected())
  {
    printf("onconnceton(): new connection [%s] from %s\n",
              conn->name().c_str(),
              conn->pererAddress().toHostPort().c_str());
    
  }
  else
  {
    printf("onconnection(): connection [%s] is down\n",
              conn->name().c_str());
  }
}


void onMessage(const libnet::TcpConnectionPtr& conn, const char* data, ssize_t len)
{
  printf("onmessage(): received %zd byutes from connection [%s]\n",
          len, conn->name().c_str());

}

int main(int argc, char const *argv[])
{
  printf("main():pid = %d\n", std::this_thread::get_id());

  libnet::InetAddress listenAddr(9989);
  libnet::EventLoop loop;

  libnet::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();
  loop.loop();
  
  return 0;
}



void on