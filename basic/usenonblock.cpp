#include<iostream>
#include<cstdlib>
#include<stdlib.h>
#include<cstdio>
#include <errno.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#define PORT 8899


int handleErr(int err, const char msg[],bool exitnow=false)
{
  if( err == -1)
  {
    printf("the errno is %d\n",errno);
    perror(msg);
    if(exitnow)exit(EXIT_FAILURE);
  }
  return err;
}
int main(int argc, char const *argv[])
{
  
  int  server,valread,valsend;
  int opt=1 ;  
  char buffer[1024]{0};
  int client_fd = handleErr(socket(AF_INET,SOCK_STREAM,0), "socket create failed");
  handleErr(
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt)),
    "set socket option failed");  
  struct sockaddr_in my_address,server_address;
  // optional bind client address
  // my_address.sin_family = AF_INET;
  // my_address.sin_port = htons(8900);
  // my_address.sin_addr.s_addr = INADDR_ANY;  
  // handleErr(
  //   bind(client_fd, (sockaddr*)&my_address, sizeof(my_address)),
  //   "set address error");  
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  handleErr(
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr),
    "serverip convert error");  
  
  handleErr(
    connect(client_fd, (sockaddr*)&server_address,sizeof(server_address)), 
    "connect error");
  
  int flags = handleErr(fcntl(client_fd, F_GETFL), "could not get file flags");
  handleErr(fcntl(client_fd, F_SETFL, flags | O_NONBLOCK), "could not set file flags");
  while (true)
  {
    valread = handleErr(read(client_fd,buffer,1024), "read error");
    if( valread > 0)std::cout<<valread<<"read from peer :"<<buffer<<std::endl;
    usleep(2000*1000);    
  }
  return 0;
}
