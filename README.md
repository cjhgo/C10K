## C10K

### c/c++ socket api
包含头文件
```c
#include <sys/socket.h> for bind/listen/accept
#include <netinet/in.h> for address_in
#include <unistd.h> for read/send
#include <arpa/inet.h> for inet_pton
```

+ socket,创建一个套接字,返回一个socket 文件描述符
`int sockfd = socket(int domain, int type, int protocol);`
  - domain参数指定网络层协议,
  AF代表address family,即网络通信地址家族
    + AF_UNIX/AF_LOCAL,本地通信协议,
    `man 7 unix : struct address_un`
    + AF_INET: ip协议, 
    `man 7 ip : struct address_in`
    + AF_INET6: ipv6协议
    `man 7 ipv6 : struct address_in6`
    + AF_IPX/AF_AX25/AF_ATMPVC/AF_ALG/AF_X25/AF_APPLETALK/AF_PACKET
    + AF_NETLINK: Kernel user interface device
  - type参数指定传输层协议, SOCK_STREAM: tcp协议, SOCK_DGRAM: udp协议
  - protocol参数用于指定额外的网络协议
+ 设置或获取套接字的属性
  ```c
  int getsockopt(int sockfd, int level, int optname,
                void *optval, socklen_t *optlen);
  int setsockopt(int sockfd, int level, int optname,
                const void *optval, socklen_t optlen);
  ```
+ bind,把套接字绑定到某个端口地址
`int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);`
用socket函数创建出来的套接字只指定了具体的domain,bind函数用于给socket分配具体的地址,地址addr是一个`sockaddr`结构体,其定义如下:
  ```c
  struct sockaddr {
      sa_family_t sa_family;
      char        sa_data[14];
  }
  ```
  但是实际传入的addr的参数的具体类型取决于socket的实际domain类型,比如如果是一个AF_INET协议的socket传入的是`struct address_in`,如果是一个AF_UNIX协议的socket传入的是`struct address_un`.
  这些不同的结构体只有`sa_family_t`这个字段是共有的,其他字段都是自定义的,无论哪种addr协议传入时候要统一类型转换为`sockaddr*`的类型.
  server和client socket都可以用这个函数绑定具体的地址,
  -  domain使用ip协议的时候,man 7 ip告诉我们,
    如果server 套接字调用listen的时候未绑定地址,或者client套接字调用connect的时候未绑定地址,
    系统都会自动给它们绑定`INADDR_ANY:free random port`这样一个地址.
    因此,server套接字要对外服务必须手动绑定端口地址,client套接字只需要知道server地址因此让系统自动绑定端口地址.
+ listen,在套接字上启动监听链接
`int listen(int sockfd, int backlog);`
使sockfd这个服务端套接字处于主动的监听状态,backlog指定允许多少个套接字等待被accept
返回值0表示调用成功,-1表示调用失败
+ accept,从监听状态的sockfd套接字上接受一个来自client的链接
`int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`
返回一个代表client的文件描述符,后续可以对这个文件描述符读写,就是对client进行读写.
`client的地址会填充到addr参数中,addrlen代表client的实际的地址长度.`
(由于要修改这个addrlen,因此这是一个指针类型的参数).
当addr不足以填充client的实际地址时,accept之后的addrlen就比传进去的大.
网络io是否阻塞发体现在这个环节,
如果是阻塞io这个调用会一直等待直到有客户端尝试建立链接.
如果是非阻塞io,当此时没有进来链接时,这个调用会直接返回`EAGAIN or EWOULDBLOCK`.
有两种方式把这个调用指定为非阻塞的.
一种是设置sockfd这个描述符的type为SOCK_NONBLOCK
一种是使用accept4,在flag参数中设置SOCK_NONBLOCK,不过这个函数是非标准的linux扩展,不能跨平台使用.
+ connect,把sockfd这个套接字和位于地址addr上的server套接字建立链接
`int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);`
返回值表明是否成功建立链接,0成功,-1失败
+ read,从一个文件描述符中读取内容
`ssize_t read(int fd, void *buf, size_t count);`
从fd中尝试读取count个byte到buf中,返回实际读取了多少byte
这个系统调用也会体现网络io是否阻塞.
当fd这个套接字目前没有消息到来的时候,
如果fd这个套接字是阻塞的调用会一直等待,如果是非阻塞的会返回`EAGAIN or EWOULDBLOCK`
+ write,向一个文件描述符写内容
`ssize_t write(int fd, const void *buf, size_t count);`
和read类似,这个系统调用也体现网络io是否阻塞
+ send,向一个套接字发送消息
`ssize_t send(int sockfd, const void *buf, size_t len, int flags);
`和write类似,但是专用于写socket,flags变量用于指定额外的选项,如果为0,等价于write
+ socket编程流程
  1. 使用socket函数创建套接字
  2. server端:bind->listen->accept->send/receive
  3. client端:connect->send/receive
  

### epoll api
`#include <sys/epoll.h>`

1. 使用`epoll_create`创建一个epoll实例,
`int epoll_create(int size);`
调用成功返回一个非负的文件描述符代表epoll 实例,调用失败返回-1.
最初的时候size参数用来通知内核调用者希望向epoll set中添加多少个文件描述符以便于内核根据这个信息有效地创建epoll 实例,Linux 2.6.8之后内核不再需要这个信息,但为了向前兼容size必须是一个正整数.
2. 使用`epoll_ctl`向epoll set中添加感兴趣的文件描述符
`int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`
在epfd这个epoll 实例上对fd这个文件描述符执行op这个操作,event配合op操作使用,调用成功返回0,失败返回-1.
op操作包括:
`EPOLL_CTL_ADD`:向epfd注册fd,并关联event事件
`EPOLL_CTL_MOD`:修改epfd中的fd关联event事件
`EPOLL_CTL_DEL`:从epdf中删除fd,此时event参数可以忽略
`epoll_event`是一个结构体,定义如下:
    ```c
    typedef union epoll_data {
          void        *ptr;
          int          fd;
          uint32_t     u32;
          uint64_t     u64;
      } epoll_data_t;

    struct epoll_event {
        uint32_t     events;      /* Epoll events */
        epoll_data_t data;        /* User data variable */
    };
    ```
    events可取的值为:
    EPOLLIN,fd可以read
    EPOLLOUT,fd可以write
    EPOLLRDHUP/EPOLLPRI/...
3. 使用`epoll_wait`等待感兴趣的事件发生
`int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);`
在epfd等待事件发生,具体发生的事件会填充到events这个指针中去,maxevents指定最多等待多少个事件,timeout以ms为单位指定等待多长时间,-1指定一直等待,0指定不等待.
调用成功返回ready的fd的数量,或者超时的时候返回0,调用失败返回-1.
#### 边缘触发和水平触发
