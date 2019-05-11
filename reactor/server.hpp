#ifndef SERVER_HPP
#define SERVER_HPP

#include "util.hpp"

class Server
{
public:
  void Run()
  {
    
    init();
    std::cout<<"server "<<fd<<" listenning\n";
    handleErr(
      listen(fd, 10000),
      "liseten connection error");
  };
  Server(int p ):port(p){};
  int getFd()const
  {
    return fd;
  };
private:
  void init()
  {
    fd=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    handleErr(fd, "socket create failed");
    int opt;
    handleErr(
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt)),
      "set socket option failed");
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    handleErr(
      bind(fd, (sockaddr*)&address, sizeof(address)),
      "set address error");      
  };
  int fd,port;  
};

#endif