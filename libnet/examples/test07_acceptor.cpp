#include "libnet/libnet.hpp"

using namespace libnet;


void newConnection(int sockfd, const InetAddress& peerAddr)
{
  LOG_INFO<<"new connection(): accept a new connection from "<<peerAddr.toHostPort().c_str()<<"\n";
  
  ::write(sockfd, "HTTP/1.1 200 OK\r\n\r\nHow are you\n", 31);
  sockets::close(sockfd);
}




int main(int argc, char const *argv[])
{
  printf("main(): begin\n");
  Logger::instance().setLogLevel(NONE);
  EventLoop loop;
  InetAddress listenAddr(9988);
  Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  loop.loop();
  return 0;
}
