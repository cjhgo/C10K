#include "libnet/libnet.hpp"

using namespace libnet;

void onConnection(const TcpConnectionPtr& conn)
{
  if(conn->connected())
  {
    // printf("onConnection():\t new connection %s from %s \n", conn->name().c_str(), 
                                                  // conn->pererAddress().toHostPort().c_str());

  }else
  {
    // printf("onConnection():\t connection %s is down \n", conn->name().c_str());

  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receivetime)
{
  

  size_t n = buf->readableBytes();
  
  // printf("onmessage(): receive %zd bytes: %s from connection[%s] at %s\n",
  //         n,
  //         res.c_str(),
  //         conn->name().c_str(),
  //         receivetime.toFormattedString().c_str());  

  //header, 19 const char* header = "HTTP/1.1 200 OK\r\n\r\n";
  std::string str("HTTP/1.1 200 OK\r\n\r\n");  
  str+= buf->retriveAsString();
  conn->send(str);
  conn->shutdown();
}

int main(int argc, char const *argv[])
{
  int threadnum = std::thread::hardware_concurrency()-1;
  if( argc > 1)
  threadnum = atoi(argv[1]);
  printf("main():pid = %d, thread num :%d\n", std::this_thread::get_id(),threadnum);

  Logger::instance().setLogLevel(NONE);
  InetAddress listenAddr(9989);
  EventLoop loop;
  TcpServer server(&loop, listenAddr);  
  
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);

  server.setThreadNum(threadnum);
  server.start();    
  
  loop.loop();
  return 0;
}

