#include<cstdlib>
#include<stdlib.h>
#include<cstdio>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
  int client_fd = socket(AF_INET,SOCK_STREAM,0), server,valread,valsend;
  int opt=1 ;  
  char buffer[1024]{0};
  handleErr(client_fd, "socket create failed");
  handleErr(
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt)),
    "set socket option failed");  
  struct sockaddr_in my_address,server_address;

  my_address.sin_family = AF_INET;
  my_address.sin_port = htons(8900);
  my_address.sin_addr.s_addr = INADDR_ANY;  
  handleErr(
    bind(client_fd, (sockaddr*)&my_address, sizeof(my_address)),
    "set address error");  
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  handleErr(
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr),
    "serverip convert error");  
  
  handleErr(
    connect(client_fd, (sockaddr*)&server_address,sizeof(server_address)), 
    "connect error");
  const char * hello = "hello server at 8899";
  valsend = send(client_fd, hello, strlen(hello),0);
  printf("%d\n", valsend);

  valsend = send(client_fd, "hello client", 12,0);
  printf("value %d send\n", valsend);
  return 0;
}
