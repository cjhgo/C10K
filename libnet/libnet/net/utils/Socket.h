#ifndef SOCKET_H
#define SOCKET_H

namespace libnet
{
class InetAddress;

class Socket
{
 public:
  explicit Socket(int sockfd):sockfd_(sockfd){};
  ~Socket();

  int fd() const { return sockfd_;}

  void bindAddress(const InetAddress& localaddr);
  void listen();
  int accept(InetAddress* peeraddr);
  void setReuseAddr(bool on);
  void shutdownWrite();
 private:
  const int sockfd_;
};
}
#endif