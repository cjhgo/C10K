#include<cstdlib>
#include<stdlib.h>
#include<cstdio>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#define PORT 8899
#define MAXEVENTS 10

void handleErr(int err, const char msg[])
{
  if( err == -1)
  {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}
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
int main(int argc, char const *argv[])
{
  struct epoll_event ev, events[MAXEVENTS];
  Server s(PORT);
  s.Run();
  int epollfd, nfds,server,client;
  int valread,valsend;;
  server = s.getFd();
  epollfd = epoll_create(1);
  handleErr(epollfd, "epoll create error");
  ev.events = EPOLLIN;
  ev.data.fd = server;
  handleErr(
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server, &ev),
    "add server in event error");
  
  struct sockaddr_in address;
  int addresslen = sizeof(address);
  while (true)
  {
    nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
    handleErr(nfds, "epoll wait error");
    std::cout<<"get ready in events number:"<<nfds<<"\t"<<events[0].data.fd<<std::endl;
    for(int i = 0; i < nfds; i++)
    {
      auto e = events[i];
      if(e.data.fd == server)
      {
        client = accept4(server,(sockaddr*)&address, (socklen_t*)&addresslen,SOCK_NONBLOCK);
        handleErr(client, "accept error");
        ev.events = EPOLLIN|EPOLLET;
        ev.data.fd = client;
        handleErr(
          epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &ev),
          "add client in event error");                
        std::cout<<"add client "<<client<<" to epoll set\n";
      }else
      {
        client = e.data.fd;        
        std::cout<<"get ready client "<<client<<std::endl;
        char buffer[1024]{0};
        valread = read(client, buffer, 1024);
        std::cout<<"read from client "<<client<<"\t"<<buffer<<std::endl;
      }      
    }
    // int a ;
    // std::cin>>a;
  }  
  return 0;
}
