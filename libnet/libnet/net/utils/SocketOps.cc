#include "libnet/base/logger.h"
#include "SocketsOps.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

namespace libnet
{
  void handleErr(int err, const char* msg)
  {
    if( err < 0)
    {

      LOG_DEBUG<<msg<<strerror(errno)<<"\n";
      // perror(ss.str().c_str());
    }
  }
namespace sockets
{

  void setNonBlockAndCloseOnExec(int sockfd)
  {
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd,F_SETFL, flags);

    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);

  }
  int createNonblockingOrDie()
  {
    int sockfd = ::socket(AF_INET, 
                          SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);

    handleErr(sockfd);
    return sockfd;
  }

  void bindOrDie(int sockfd, const struct sockaddr_in& addr)
  {
    int ret = ::bind(sockfd, (const sockaddr*)(&addr), sizeof(addr));
    handleErr(ret);
  }

  void listenOrDie(int sockfd)
  {
    int ret =  ::listen(sockfd, 100000);
    //SOMAXCONN);
    handleErr(ret);
  }

  int  accept(int sockfd, struct sockaddr_in* addr)
  {
    socklen_t addrlen = sizeof*addr;
    int connfd = ::accept4(sockfd, (sockaddr*)addr, 
                    &addrlen,SOCK_NONBLOCK|SOCK_CLOEXEC);
    handleErr(connfd);
    return connfd;
  }

  void close(int sockfd)
  {
    int res = ::close(sockfd);
    handleErr(res);
  }

  void shutdownWrite(int sockfd)
  {
    int res = ::shutdown(sockfd,SHUT_WR);
    handleErr(res);
  }
  void toHostPort(char* buf, size_t size,
                const struct sockaddr_in& addr)
  {
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
    uint16_t port = networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
  }
  
  void fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr)
  {
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    int res = ::inet_pton(AF_INET, ip, &addr->sin_addr);
    handleErr(res);
  }

  struct sockaddr_in getLocalAddr(int sockfd)
  {
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = sizeof(localaddr);
    int ret = ::getsockname(sockfd, (sockaddr*)(&localaddr), &addrlen);
    handleErr(ret);
    return localaddr;
  }


}
}