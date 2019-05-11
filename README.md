[toc]

## C10K

### c/c++ socket api
包含头文件
```c
#include <sys/socket.h> for bind/listen/accept
#include <netinet/in.h> for address_in
#include <unistd.h> for read/send
#include <arpa/inet.h> for inet_pton
```

+ socket,
创建一个套接字,返回一个socket 文件描述符
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
+ bind,
把套接字绑定到某个端口地址
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
+ listen,
在套接字上启动监听链接
`int listen(int sockfd, int backlog);`
使sockfd这个服务端套接字处于主动的监听状态,backlog指定允许多少个套接字等待被accept
返回值0表示调用成功,-1表示调用失败
+ accept,
从监听状态的sockfd套接字上接受一个来自client的链接
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
+ connect,
把sockfd这个套接字和位于地址addr上的server套接字建立链接
`int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);`
返回值表明是否成功建立链接,0成功,-1失败
如果sockfd是一个nonblock socket,那么connect调用会返回`EINPROGRESS`,表明无法马上即建立链接.
connect这一步没必要使用nonblock,握手链接有必要等待,这个时候就不能在`socket`调用的时候设置nonblock,
此时就用到了`fcntl`这个函数来设置socket的属性为`O_NONBLOCK`.
+ read,
从一个文件描述符中读取内容
`ssize_t read(int fd, void *buf, size_t count);`
从fd中尝试读取count个byte到buf中,返回实际读取了多少byte
这个系统调用也会体现网络io是否阻塞.
当fd这个套接字目前没有消息到来的时候,
如果fd这个套接字是阻塞的调用会一直等待,如果是非阻塞的会返回`EAGAIN or EWOULDBLOCK`
+ write,
向一个文件描述符写内容
`ssize_t write(int fd, const void *buf, size_t count);`
和read类似,这个系统调用也体现网络io是否阻塞
+ send,
向一个套接字发送消息
`ssize_t send(int sockfd, const void *buf, size_t len, int flags);
`和write类似,但是专用于写socket,flags变量用于指定额外的选项,如果为0,等价于write
**对于read来说block是等待peer发送数据,对于send/write来说block是等待缓冲区有空间**
(缓冲区到底在哪里,是什么,就依赖于对os的理解了)
+ socket编程流程
  1. 使用socket函数创建套接字
  2. server端:bind->listen->accept->send/receive
  3. client端:connect->send/receive
  


有两个环节体现阻塞与非阻塞
+ 第一个是server socket 进行accept操作的时候,server socket是否会一直等待直到有客户端进来.
这个地方通过创建server socket的时候设置
+ 第二个是对client socket 进行read/send操作的时候,read是否会一直等待peer发送数据,send是否会一直等待peer接收数据.
这个地方通过使用`accept4`设置或者用`fcntl`设置client
+ nonblock client socket 调用connect的时候会返回`EINPROGRESS`,client socket connect的时候没必要nonblock

**nonblock socket调用read/send可能block的时候,EAGAIN是设置在errno这个变量中的,read/send本身返回的是-1**
(想想确实应该这样,EAGAIN的值是11,如果返回11岂不是和读了11byte混淆了)

怎样用`fcntl`来设置socket为nonblock
```c
int flags = handleErr(
      fcntl(client_fd, F_GETFL), 
      "could not get file flags");
handleErr(
      fcntl(client_fd, F_SETFL, flags | O_NONBLOCK), 
      "could not set file flags");
```
第一步要获取flag,第二步才能设置,至于fcntl函数的详细理解,甚至于setsockopt中的level的概念,又是另一个话题了.


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

linux在`/proc/sys/fs/epoll/max_user_watches`指定了系统中一个user启动的所有epoll实例能够监听的文件描述符的上限.


在`/proc/[pid]/fdinfo`中可以看到一个进程的epoll set中的文件描述符

#### 边缘触发和水平触发
epoll的事件分发接口提供边缘触发(edge-triggered,et)和水平触发(level-triggered,lt)两种机制,它们的区别可以在下边的例子场景中体现出来:

1. epoll实例中注册了用于read的一个文件描述符,rfd
2. 另一端向rfd中写入了2kb的数据
3. epoll_wait调用结束返回一个可读的rfd
4. 程序从rfd中读去了1kb的数据(还有1kb数据没读取)
5. 再次调用epoll_wait,等待剩余的1kb数据

在edge-triggered模式下,epoll实例只在监听的文件描述符**状态发生改变的时候**(从不可读变成可读)发出一次通知.
因此在第5步中,尽管剩余的1kb数据已经送达,但是epoll实例不会再次返回可读事件,同时另一端也在等待自己读完所有的2kb数据,
那么这个步骤5中的epoll_wait就会一直等待下去(hang).
即边缘触发模式只在状态边缘通知一次事件,read消费了这次事件后如果没有读完所有的内容,那么后来的epoll_wait就block indefinitely.

使用边缘触发模式的时候,应用程序应该使用非阻塞的文件描述符,因为阻塞的文件描述符读写的时候会使应用程序产生饥饿现象,详见后边.
具体而言,规则如下
1. 使用非阻塞的文件描述符
2. 只在发生EAGAIN的情况下等待事件
(发生eagain说明读完了缓冲中的数据,上边例子中发了2kb,读了1kb就不会返回eagain)


水平触发模式下,只要文件描述符可读或可写,epoll实例就会分发相应的事件,这种情况下的epoll相当于一个更高效的poll.
(也就是说epoll相对于poll提供了et模式).


epoll提供了`EPOLLONESHOT`事件,使得某个文件描述符被通知一次后内部后续就不再提供通知,程序员自己要再次调用EPOLL_CTL_MOD来启用通知.

#### epoll Q&A
0. Q:epoll set中用什么来区分程序注册进来的文件描述符?
A:用the file descriptor number + the open file description
(后者又叫做open file handle, 是内核对打开的文件的内部表示)
1. Q:在一个epoll实例中把同一个文件描述符注册两次会怎样?
A:也许会收到`EEXIST`报错.不过通过`dup`调用可以向epoll实例中再次注册一个文件描述符.
2. Q:两个epoll实例能监听同一个文件描述符吗?如果能,事件是怎么报告的?
A:能,两个实例都收到事件.
3. Q:一个epoll自己能够被轮询吗?
A:可以(但不能轮询自己).
4. Q:一个epoll实例轮询自己会怎样?即把自己通过epoll_ctl放入自己的epoll_set
A:会报错`EINVAL`
5. Q:可以通过unix socket把一个epoll文件描述符发送给另一个进程吗?
A:可以,但这样做没意义,因为接收进程收到epoll set没有原来的文件描述符.
6. Q:关闭一个文件描述符会使得它从epoll set中自动移出吗?
A:会,但同时要考虑下述情况.文件描述符是对打开的文件的引用.
通过`dup/fcntl/F_DUPFD/fork`会造成对同一个打开的文件的多个引用(多个文件描述符指向同一个文件).只有当所有的文件描述符都close之后,打开的文件才会被真正close.
7. Q:epoll_wait之间发生了多次事件,这些事件是分别报告还是组合报告?
A:组合报告
8. Q:对文件描述符的操作会影响已经收集但是还没有报告的事件吗?
A:没看懂这个问题,,,
9. Q:当使用et模式的时候,我需要持续读写直到遇到了EAGAIN吗?
A:使用et模式的时候,从epoll_wait中收到事件表明文件描述符可以用于你所请求的io操作.(ready for requested io).
在读写产生EAGAIN之前,你都要认为这个文件描述符是可读写的.(因此是的).
不过,何时和如何使用这个文件描述符完全取决于你.
如果udp协议,那确实是这样,判断读完或写完的唯一方式就是一直读直到产生了EAGAIN.
如果是tcp协议,还有其他判断读写空间消耗完毕的方式.比如你请求读2kb的数据,但是read告诉你实际读了1kb的数据,那么你当然知道,已经读完了.
(为什么,udp不能这样,,)
#### 使用epoll常见的陷阱和解决方法
+ 饥饿现象,为什么要用非阻塞socket
epoll+阻塞socket会产生饥饿现象.原因是这样的,
使用epoll的时候我们一般是用一个epoll实例监听多个socket,即一个主线程的io-loop循环.
使用阻塞socket,主线程有可能陷入等待,而此时其他可读写的socket无法被处理,得不到cpu资源,这些本该被处理的socket就处于饥饿状态.
因此要使用非阻塞socket.
还有一种情况也说明使用非阻塞socket的必要性.
epoll报告了可读事件,但是由于校验失败,数据被丢弃了,此时去读如果是阻塞的socket就会一直等待下去.
+ 使用event cache时容易出现的问题
If  you use an event cache or store all the file descriptors returned from epoll_wait(2), 
then make sure to provide a way to mark its closure dynamically (i.e., caused by a previous event's processing)

### 其他多路复用,poll/select/ppoll/ppselect
除了epoll之外,linux还提供了poll/select用于多路复用
#### poll
`#include <poll.h>`
`int poll(struct pollfd *fds, nfds_t nfds, int timeout);`

fds是一个数组,nfds指明了这个数组的长度
pollfd结构体定义如下:
```c
struct pollfd {
    int   fd;         /* file descriptor */要监听的文件描述符
    short events;     /* requested events */感兴趣的事件
    short revents;    /* returned events */返回时填充了监听到的事件
};
```
#### select
```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, struct timeval *timeout);        
void FD_CLR(int fd, fd_set *set);从set中去除fd
int  FD_ISSET(int fd, fd_set *set);判断fd是否在set中
void FD_SET(int fd, fd_set *set);把fd加入到set中
void FD_ZERO(fd_set *set);清除一个set
```        
监听readfds中文件描述符的可读事件,writefds中的可写事件,exceptfd中的异常事件,三个set皆可为空.
nfds+1指定了三个set的最高size
select调用返回三个set中发生事件的fd的总数

提供了4个宏来操作这些set

执行`select(0,null,null,null,time)`可以用于亚秒精度的sleep

select只能监听不超过`FD_SETSIZE`数量的文件描述符,glibc实现中这个值是1024,也就是说select智能监听不超过1024个文件描述符,poll没有这个限制.

select中的fd_set是一个`fixed size buffer`,poll虽然没有1024的限制但是也要在参数中指定nfds,而epoll完全是动态add/remove的set.
并且轮询结束的时候,select/poll都需要O(n)的遍历set来判断事件是否发生,epoll会把发生的事件填充到指针参数中,不需要O(n)的遍历.
所以epoll>poll>select.不过在程序的可移植性方面,这个顺序是反过来的.
#### 加入信号机制的多路复用
```c
int pselect(int nfds, fd_set *readfds, fd_set *writefds,
              fd_set *exceptfds, const struct timespec *timeout,
              const sigset_t *sigmask);

int ppoll(struct pollfd *fds, nfds_t nfds,
        const struct timespec *tmo_p, 
        const sigset_t *sigmask);

int epoll_pwait(int epfd, struct epoll_event *events,
              int maxevents, int timeout,
              const sigset_t *sigmask);
```
为什么要加入信号机制?
>man pselect:
>The reason that pselect() is needed is that if one wants to wait for either a signal or for a file descriptor to become ready, then an atomic test is needed to prevent race condi‐
tions.  (Suppose the signal handler sets a global flag and returns.  Then a test of this global flag followed by a call of select() could hang indefinitely if the  signal  arrived
just after the test but just before the call.  By contrast, pselect() allows one to first block signals, handle the signals that have come in, then call pselect() with the desired
sigmask, avoiding the race.)
### libevent
libevent , 异步io?

libevent提供了一个跨平台可移植的轮询操作api.
select太低效,linux上有epoll,BSD上有kqueue.

A tiny introduction to asynchronous IO
http://www.wangafu.net/~nickm/libevent-book/01_intro.html
http://www.wangafu.net/~nickm/libevent-2.1/doxygen/html/


### 参考资料

https://idea.popcount.org/2017-02-20-epoll-is-fundamentally-broken-12/
https://idea.popcount.org/2017-03-20-epoll-is-fundamentally-broken-22/

http://www.kegel.com/c10k.html


unix 网络编程 卷1 套接字编程 在线版本
http://www.masterraghu.com/subjects/np/introduction/unix_network_programming_v1.3/toc.html


一份100页的套接字编程介绍材料,相较于史蒂文斯的900页的教材,这个算精简的
http://beej.us/guide/bgnet/pdf/bgnet_A4.pdf
https://www.gta.ufrj.br/ensino/eel878/sockets/index.html
http://beej.us/guide/bgnet/html/single/bgnet.html
