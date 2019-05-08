#include<cstdlib>
#include<stdlib.h>
#include<cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#define PORT 8899


void handleErr(int err, const char msg[])
{
  if( err == -1)
  {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}
int main(int argc, char const *argv[])
{
  int server_fd = socket(AF_INET,SOCK_STREAM,0), client,valread,valsend;
  int opt=1 ;
  char buffer[1024]{0};
  
  handleErr(server_fd, "socket create failed");
  handleErr(
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt)),
    "set socket option failed");
  
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;
  int addresslen = sizeof(address);
  handleErr(
    bind(server_fd, (sockaddr*)&address, sizeof(address)),
    "set address error");
  
  handleErr(
    listen(server_fd, 5),
    "liseten connection error");
  
  client = accept(server_fd, (sockaddr*)&address,(socklen_t*)&addresslen);
  handleErr(client, "accept error");
  std::cout<<client<<std::endl;
  valread = read(client, buffer, 1024);
  printf("%s\n", buffer);

  valsend = send(client, "hello client", 12,0);
  printf("value %d send\n", valsend);
  return 0;
}
