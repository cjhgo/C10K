#include <signal.h>
#include "util.hpp"
#include "server.hpp"
#include "ThreadPool/ThreadPool.hpp"
#define MAXEVENTS 100
#define PORT 8899

int server, epollfd;

void handleEvent(int fd)
{
  int client = fd;

  signal(SIGPIPE, SIG_IGN);
  if( fd == server)
  {
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
  }else
  {    
    std::cout<<"get ready client "<<client<<std::endl;
    char buffer[1024]{0};
    int valread = handleErr(
                    read(client, buffer, 1024),
                    "read from client error");
    std::cout<<"the read res is "<<valread<<std::endl;
    perror("???");
    std::cout<<"read from client "<<client<<"\t"<<buffer<<std::endl;
    std::stringstream ss;
    ss<<"hello client ,i receive your :"<<std::string(buffer);;
    std::string s = ss.str();
    std::cout<<" will send "<<s<<" to client "<<client<<std::endl;
    int n = s.size(), sendlen=0;
    while( sendlen < n)
    {
      int valsend=handleErr(
                      send(client,s.c_str(),n,0),
                      "send to client error");
      if( valsend > 0)sendlen += valsend;
      else if( valsend == 0 || errno == 32)
      {
        std::cout<<"client "<<client<<"closed"<<std::endl;
        break;
      }
      else if( errno == EWOULDBLOCK)
      {
        usleep(2000*1000);
      }
    }
  }      
}



int main(int argc, char const *argv[])
{

  struct epoll_event ev, events[MAXEVENTS];
  Server s(PORT);
  s.Run();
  
  server = s.getFd();  
  epollfd = epoll_create(1);
  handleErr(epollfd, "epoll create error");
  ev.events = EPOLLIN;
  ev.data.fd = server;
  handleErr(
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server, &ev),
    "add server in event error");
  
  ThreadPool tp(std::thread::hardware_concurrency());
  tp.run();
  int nfds;
  while (true)
  {
    nfds = handleErr(
            epoll_wait(epollfd, events, MAXEVENTS, -1), 
            "epoll wait error");
    std::cout<<"get ready in events number:"<<nfds<<std::endl;
    for(int i = 0; i < nfds; i++)
    {
      auto e = events[i];      
      tp.add(Task(handleEvent, e.data.fd));
    }
  }      
  tp.join();  
  return 0;
}
