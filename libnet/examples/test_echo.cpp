#include <cstdio>
#include <iostream>
#include <thread>
#include "libnet/net/EventLoop.h"
#include "libnet/net/Acceptor.h"
#include "libnet/net/utils/InetAddress.h"
#include "libnet/net/utils/SocketsOps.h"



void newConnection(int sockfd, const libnet::InetAddress& peerAddr )
{
  printf("newconnection: accept a new connection from %s\n",
              peerAddr.toHostPort().c_str());
  
  ::write(sockfd, "how are you?\n", 13);
  libnet::sockets::close(sockfd);
}

int main(int argc, char const *argv[])
{
  printf("main():pid = %d\n", std::this_thread::get_id());

  libnet::InetAddress listenAddr(9989);
  libnet::EventLoop loop;
  
  libnet::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();
  return 0;
}

