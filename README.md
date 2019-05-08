## C10K

### c/c++ socket api
`#include <sys/socket.h>//包含头文件`

+ 创建一个套接字,返回一个socket 文件描述符
`int sockfd = socket(int domain, int type, int protocol);`
domain参数指定网络层协议, AF_INET: ip协议, AF_INET6: ipv6协议
type参数指定传输层协议, SOCK_STREAM: tcp协议, SOCK_DGRAM: udp协议
protocol参数用于指定额外的网络协议
+ 设置或获取套接字的属性
```c
int getsockopt(int sockfd, int level, int optname,
              void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname,
              const void *optval, socklen_t optlen);
```
+ 把套接字绑定到某个端口地址,bind
`int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);`

+ 在套接字上启动监听链接,listen
`int listen(int sockfd, int backlog);`

+ 在套接字上接受一个链接,accept
`int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

+ 把sockfd这个套接字和某个地址上的套接字建立链接,connect
`int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);`
返回值表明是否成功建立链接,0成功,-1失败

### epoll api
