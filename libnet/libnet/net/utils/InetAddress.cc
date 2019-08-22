#include "SocketsOps.h"
#include "InetAddress.h"


using namespace libnet;

static const in_addr_t kInaddrAny= INADDR_ANY;

InetAddress::InetAddress(uint16_t port)
{
  
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
  addr_.sin_addr.s_addr = INADDR_ANY;
  addr_.sin_port = sockets::hostToNetwork16(port);  
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
  sockets::fromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::toHostPort() const
{
  char buf[32];
  sockets::toHostPort(buf,sizeof buf, addr_);
  return buf;
}