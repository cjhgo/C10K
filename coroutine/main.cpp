#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <signal.h>
#include "util.hpp"
#include "server.hpp"
#include "fiber/fiber_group.h"
#include "fiber/fiber_control.h"
#define MAXEVENTS 100
#define PORT 8901

int server, epollfd;
Fiber_group fg;
std::vector<int> rfds;



void handleClient(int fd)
{  
  std::string input;
  std::stringstream ss;
  while(true)
  {
    std::cout<<"handle client "<<fd<<std::endl;
    if(input.size() > 3 && input.substr(input.size()-4) == "eof\n")
    {
      ss<<"hello client ,i receive your :"<<input;      
      std::cout<<" will send "<<input<<" to client "<<fd<<std::endl;
      input = ss.str();
      int n = input.size(), sendlen=0;
      auto tosend = input.c_str();
      while( sendlen < n)
      {
        int valsend=handleErr(
                        send(fd,tosend+sendlen,n-sendlen,MSG_NOSIGNAL),
                        "send to client error");
        if( valsend > 0)sendlen += valsend;
        else if(errno == 32)
        {
          std::cout<<valsend<<" client "<<fd<<"closed"<<errno<<std::endl;
          break;
        }
        else if( errno == EWOULDBLOCK)
        {
          Fiber_Control::yield();
        }
      }      
      break;
    }
    while (true)
    {
      if(std::find(rfds.begin(),rfds.end(), fd) == rfds.end())
      {
        Fiber_Control::yield();
      }
      char buffer[20]{0};
      int valread = handleErr(read(fd, buffer, 20),"read from client error");
      std::cout<<"from client"<<fd<<" read "<<buffer<<"\t"<<valread<<"\t"<<errno<<std::endl;
      // if( valread == -1 && (errno == EWOULDBLOCK || errno == EAGAIN) )
      if( valread == -1 )
      {
        // usleep(2000*1000);
        Fiber_Control::yield();
      }else if( valread == 0)
      {
        break; 
      }else if( valread > 0)
      {
        ss<<std::string(buffer);
        input += ss.str();
        ss.str("");
        if(valread < 20)break;//read finish
        std::cout<<"finish reading once\n";
      }
    }    
  }
}


void handleAccept(int fd)
{
  int client;
  struct sockaddr_in address;
  int addresslen = sizeof(address);
  client = handleErr(
            accept4(server,(sockaddr*)&address, (socklen_t*)&addresslen,SOCK_NONBLOCK), 
            "accept error");
  struct epoll_event ev;    
  ev.events = EPOLLIN|EPOLLET;
  ev.data.fd = client;
  handleErr(
    epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &ev),
    "add client in event error");                
  std::cout<<"add client "<<client<<" to epoll set\n";
  auto fun = std::bind(handleClient,client);
  fg.launch(fun);
}


void ioloop()
{
  struct epoll_event ev, events[MAXEVENTS];
  epollfd = epoll_create(1);
  handleErr(epollfd, "epoll create error");
  ev.events = EPOLLIN;
  ev.data.fd = server;
  handleErr(
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server, &ev),
    "add server in event error");
    
  
  int nfds;
  while (true)
  {
    nfds = handleErr(
            epoll_wait(epollfd, events, MAXEVENTS, -1), 
            "epoll wait error");
    std::cout<<"get ready in events number:"<<nfds<<std::endl;
    rfds.clear();
    for(int i = 0; i < nfds; i++)
    {
      auto e = events[i];      
      if(e.data.fd == server)
      {
        handleAccept(e.data.fd);
      }else
      {
        rfds.push_back(e.data.fd);
      }
    }
    Fiber_Control::yield();
  }      
}

int main(int argc, char const *argv[])
{

  
  Server s(PORT);
  s.Run();
  
  server = s.getFd();  
  fg.launch(ioloop);
  fg.join();
  return 0;
}
