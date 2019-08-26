[toc]

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
使sockfd这个服务端套接字处于主动的监听状态,
backlog指定指定了由内核代为accept的socket缓冲的数量,超过backlog之后新进来的clinet socket都被拒绝
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
**如果返回0则说明读到了文件的eof,对于一个套接字则表明peer close事件**
这个系统调用也会体现网络io是否阻塞.
当fd这个套接字目前没有消息到来的时候,
如果fd这个套接字是阻塞的调用会一直等待,如果是非阻塞的会返回`EAGAIN or EWOULDBLOCK`
+ recv,
从一个套接字中接收消息
+ write,
向一个文件描述符写内容
`ssize_t write(int fd, const void *buf, size_t count);`
和read类似,这个系统调用也体现网络io是否阻塞
+ send,
向一个套接字发送消息
`ssize_t send(int sockfd, const void *buf, size_t len, int flags);
`和write类似,但是专用于写socket,flags变量用于指定额外的选项,如果为0,等价于write
如果向一个broken pipe(比如peer close)执行send写操作,写进程会被`SIGPIPE`信号term,
同时errno设置为`EPIPE`.
所以`如果不处理`SIGPIPE`信号,写的过程中client关闭会造成server被信号杀死`.
有两种方法处理`SIGPIPE`信号,
一种是在main中`signal(SIGPIPE, SIG_IGN);`,这种控制粒度是全局的.
另一种是在flags中设置`MSG_NOSIGNAL`,这种控制只在这个send调用中生效.
+ 关于scatter/gahter io:readv,writev,一次操作多个iovec
  ```c
  ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
  把从fd中读出的内容依次读到从iov地址处开始的iovcnt个iovec中
  填充满第0个接着填充第1个
  ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
  struct iovec {
      void  *iov_base;    /* Starting address */
      size_t iov_len;     /* Number of bytes to transfer */
  };
  从iov地址开始处依次读取iovcnt个iovec中的内容,写入到write中   
  ```
+ socket编程流程
  1. 使用socket函数创建套接字
  2. server端:bind->listen->accept->send/receive
  3. client端:connect->send/receive
  
#### socket阻塞在哪里
要想理解非阻塞之后,socket的读写行为,首先要明白
socket什么情况下会阻塞?
对于读操作,socket会一直阻塞直到缓冲区中有数据
如果网络拥塞,或者对方没有消息,
那么缓冲区中就没有数据,
socket的读操作就要一直等
对于写操作,socket会一直阻塞直到缓冲区中有空间
如果网络拥塞,收不到ack或者发的太快,缓冲区被填满然后对方来不及收,
那么缓冲区就会满,
socket的写操作就要一直等
所以,**socket读写是阻塞在缓冲区上的**
而不是阻塞在对方的读/写上,只不过对方的读写会改变缓冲区的状态.
毕竟,socket并没有办法知道对方是否读写了,只观察到了socket缓存区的变化.
以及,无论是阻塞的写操作还是非阻塞的写操作,都有可能出现没写完的情况
只不过对于阻塞的写操作,可以用while循环把消息一直写完,
对于非阻塞的写操作,要配合epoll等待下一次epollout的发生才能接着写.

>所以,我原来的这个理解,就显得不太精确了.
**对于read/recv来说block是等待peer发送数据,对于send/write来说block是等待缓冲区有空间**
socket读写是阻塞在缓冲区上的,而不是对方进行了读写.
也许对方没有读写,只是之前的消息经过拥塞的网络现在可读写了.
(缓冲区到底在哪里,是什么,就依赖于对os的理解了)


有两个环节体现阻塞与非阻塞
+ 第一个是server socket 进行accept操作的时候,server socket是否会一直等待直到有客户端进来.
这个地方通过创建server socket的时候设置
+ 第二个是对client socket 进行read/send操作的时候,read是否会一直等待peer发送数据,send是否会一直等待peer接收数据.
这个地方通过使用`accept4`设置或者用`fcntl`设置client
+ nonblock client socket 调用connect的时候会返回`EINPROGRESS`,client socket connect的时候没必要nonblock

**nonblock socket调用read/send可能block的时候,EAGAIN是设置在errno这个变量中的,read/send本身返回的是-1**
(想想确实应该这样,EAGAIN的值是11,如果返回11岂不是和读了11byte混淆了)

#### 怎样用`fcntl`来设置socket为nonblock
```c
int flags = handleErr(
      fcntl(client_fd, F_GETFL), 
      "could not get file flags");
handleErr(
      fcntl(client_fd, F_SETFL, flags | O_NONBLOCK), 
      "could not set file flags");
```
第一步要获取flag,第二步才能设置,至于fcntl函数的详细理解,甚至于setsockopt中的level的概念,又是另一个话题了.

#### 如何处理关闭的套接字
+ 向一个closed socket进行写操作会发出`SIGPIPE`信号,并且设置errno为EPIPE
+ 从一个closed socket进行读操作会得到`eof`,0返回值

程序怎么检测到套接字关闭了呢
>服务器端会收到EPOLLIN或EPOLLHUP事件，为了统一处理，可以在检测到EPOLLHUP时走与EPOLLIN相同的流程，这样上层会recv返回0，从而知道对端从容关闭<br>
test for both POLLIN and POLLHUP, and rely on the subsequent read() to tell you whether you reached EOF.

https://www.greenend.org.uk/rjk/tech/poll.html

pollin&read 0 或者 pollhup
然后走相同的流程


### 从网线到网卡到socket,tcp/ip栈是怎样工作的
Understanding Linux Network Internals

https://www.linuxjournal.com/content/queueing-linux-network-stack

https://www.cubrid.org/blog/understanding-tcp-ip-network-stack

https://www2.cs.duke.edu/ari/trapeze/freenix/node6.html

https://eklitzke.org/how-tcp-sockets-work
从网卡复制到kernel
从kernel复制到user space

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
    EPOLLIN,fd可以从缓冲区中读数据了
    EPOLLOUT,fd可以向缓冲区中写数据了
    EPOLLRDHUP/EPOLLPRI/...
3. 使用`epoll_wait`等待感兴趣的事件发生
`int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);`
在epfd等待事件发生,具体发生的事件会填充到events这个指针中去,maxevents指定最多等待多少个事件,timeout以ms为单位指定等待多长时间,-1指定一直等待,0指定不等待.
调用成功返回ready的fd的数量,或者超时的时候返回0,调用失败返回-1.

linux在`/proc/sys/fs/epoll/max_user_watches`指定了系统中一个user启动的所有epoll实例能够监听的文件描述符的上限.


在`/proc/[pid]/fdinfo`中可以看到一个进程的epoll set中的文件描述符

#### 边缘触发和水平触发,区别,及使用原则
epoll的事件分发接口提供边缘触发(edge-triggered,et)和水平触发(level-triggered,lt)两种机制,设置方法如下
```c
ev.events = EPOLLIN|EPOLLOUT;//默认水平触发
ev.events = EPOLLIN|EPOLLOUT|EPOLLET;//设置为边缘触发
```
>The events member is a bit mask composed using the following available event types:

ev.events是一个bit掩码字段,默认采用水平触发模式,通过`EPOLLET`可以设置为边缘触发模式.
也就是说水平触发是epoll的默认模式,不需要专门设置,并没有`epolllt`这个选项.
以及,**这个模式是设置在fd上对整个fd生效的**,不存在说分别设置,让读事件水平触发,写事件边缘触发.

在level-trigger(水平触发)模式模式下,只要文件描述符可读或可写,epoll实例就会分发相应的事件,这种情况下的epoll相当于一个更高效的poll.
(也就是说epoll相对于poll提供了et模式).
在edge-triggered(边缘触发)模式下,epoll实例只在监听的文件描述符**状态发生改变的时候**(从不可读变成可读)发出一次通知.
>Edge vs level triggered refers to how a change in an fd's state is reflected in userspace: level triggering will cause repeated wakeups while the condition remains true, whereas edge triggering will cause exactly one, until the monitored state flips from true to false and back again.

什么意思呢,这个图可以比较直观的表达一下:
![触发比较](images/trigger.png)

可以看到,
+ 对于epollout事件,只要写缓冲区不满,那么都是可写的,
如果采用边缘触发,只在写缓冲区状态从不可写变(满)为可写(不满)的时刻报告一次
如果采用水平触发,只要写缓冲区不满,epoll_wait就立即返回报告可写事件
*但是我们可能并不关系或不需要往缓冲区里写*
比如,平时写缓冲区是有空间的,但是程序并不需要发消息,水平触发还是会报告epollout
+ 对于epollin事件,只要读缓冲区非空,那么都是可读的
如果采用水平触发,只要读缓冲区非空,epoll_wait就立即返回报告可读事件
如果采用边缘触发,只在读缓冲区从不可读(空)变为可读(非空)的时候报告一次
那么如果一次没读完,剩下的数据并不再次触发epollin事件


也就是说
>When using epoll with level triggered notifications, 
you will get constant EPOLLOUT notifications on a socket 
except for those brief periods of time that the output buffer for the socket is actually full. 
The impact of this is that your event loop will wake up 
even if you have nothing to send, and there was no data to receive either.

使用水平触发,会一直报告不必要的epollout可写事件,这个叫做`不必要的wake up` 
使用边缘触发,一次没读完的数据不会再次触发epollin可读事件,这个叫做 `饥饿`
或者说,问题在于
对于读缓冲区我们希望有数据就读,关注可读的状态
对于写缓存区我们只关系它从满变成不满,可写了,关注状态变化


man epoll中举了这个例子,来说明边缘触发

<blockquote>
1. The file descriptor that represents the read side of a pipe (rfd) is registered on the epoll instance. <br>
2. A pipe writer writes 2 kB of data on the write side of the pipe.<br>
3. A call to epoll_wait(2) is done that will return rfd as a ready file descriptor.<br>
4. The pipe reader reads 1 kB of data from rfd.<br>
5. A call to epoll_wait(2) is done.<br>
<br>
If  the  rfd file descriptor has been added to the epoll interface using the EPOLLET (edge-triggered) flag, the call to epoll_wait(2) done in step 5 will probably hang despite the
available data still present in the file input buffer; meanwhile the remote peer might be expecting a response based on the data it already sent.  The  reason  for  this  is  that
edge-triggered  mode delivers events only when changes occur on the monitored file descriptor.  So, in step 5 the caller might end up waiting for some data that is already present
inside the input buffer.  In the above example, an event on rfd will be generated because of the write done in 2 and the event is consumed in 3.  Since the read operation done  in
4 does not consume the whole buffer data, the call to epoll_wait(2) done in step 5 might block indefinitely.<br>
<br>
An application that employs the EPOLLET flag should use nonblocking file descriptors to avoid having a blocking read or write starve a task that is handling multiple file descrip‐
tors.  The suggested way to use epoll as an edge-triggered (EPOLLET) interface is as follows:
</blockquote>
1. epoll实例中注册了用于read的一个文件描述符,rfd
2. 另一端向rfd中写入了2kb的数据
3. epoll_wait调用结束返回一个可读的rfd
4. 程序从rfd中读去了1kb的数据(还有1kb数据没读取)
5. 再次调用epoll_wait,等待剩余的1kb数据

如果向epoll中添加rfd的时候,设置了epollet标志位,让epoll对rfd的可读事件按照边缘触发模式来报告,\
在第5步中,尽管剩余的1kb数据已经送达,但是epoll实例不会再次返回可读事件,同时另一端也在等待自己读完所有的2kb数据,\
那么这个步骤5中的epoll_wait就会一直等待下去(hang).\
即边缘触发模式只在状态边缘通知一次事件,read消费了这次事件后如果没有读完所有的内容,那么后来的epoll_wait就block indefinitely.\
使用边缘触发模式的时候,应用程序应该使用非阻塞的文件描述符,因为阻塞的文件描述符读写的时候会使应用程序产生饥饿现象.

为了避免这一点,边缘触发模式下,读操作要一直进行直到返回EAGIN,清空读缓冲区\
否则剩下的消息会影响后续消息的接收,造成饥饿现象.\
而`read until EAGAIN`这个操作只有非阻塞套接字才有可能\
所以,**边缘触发必须要配合非阻塞套接字来使用**\
而对于*水平触发模式,则使用阻塞套接字也可以*\
因为阻塞套接字虽然读写会阻塞,\
但是由于<U>是收到了epoll返回的事件才进行读写的</U>\
所以,实际进行读写的时候,阻塞套接字并不会阻塞,可以直接读写\
再加上没读完的数据epoll会再次报告,就不会有上边的问题.\
但是确实存在一种可能,虽然epoll返回了可读事件,但是tcp又把数据丢掉了(比如校验失败?)\
这时,水平触发下也不适合用阻塞套接字了.\

所以,用了epoll这种多路复用,当然就应该相应地配套使用非阻塞套接字.\
额外指出**边缘触发必须要配合非阻塞套接字来使用**这一点其实意在强调\
边缘触发时,读操作一定要把读缓冲区中的数据读空才不影响后续的数据.



那么,网络程序该怎样合理的使用这两种模式呢?

具体而言,
使用边缘触发的规则如下,处理好可读只触发一次这个情况
1. 使用非阻塞的文件描述符
2. 只在发生EAGAIN的情况下等待事件
(发生eagain说明读完了缓冲中的数据,上边例子中发了2kb,读了1kb就不会返回eagain)
自己处理好一次读不完,那么直到读缓冲区空了才再次触发epollin事件的情况(read until eagain)
    >Enable edge triggered notifications, and leave EPOLLOUT enabled all the time. Now, you will only get EPOLLOUT notifications when the system changes from a output buffer full state to non-full state. However, EPOLLIN is now also also edge triggered, which means you only get one EPOLLIN notification until you drain the input buffer(by read until EAGAIN).
3. 总结一下的话,推荐使用边缘触发模式,然后对于读操作,要一直读直到返回了EAGAIN,那么就不会出现没读完进而饥饿现象了.

如果使用默认的水平触发,
>epoll is simply a faster poll

不过,要处理好一直报告epollout这个情况
只在eagain的情况下add epollout事件
发送完毕之后,从epollset中remove掉epollout事件
>Disable EPOLLOUT until those times you actually get EWOULDBLOCK / EAGAIN. At that point, you enable EPOLLOUT, and disable it again once your send is complete


对于server fd,由于只关心可读的accept,没有epollout事件,用默认的水平触发进行了
对于client fd,既关系可读事件,又关心可写事件,用边缘触发+非阻塞+read_until_eagain吧

参考这两篇文章
https://stackoverflow.com/a/51757553

epoll：EPOLLLT和EPOLLET的区别
https://blog.csdn.net/daiyudong2020/article/details/50439029



epoll提供了`EPOLLONESHOT`事件,使得某个文件描述符被通知一次后内部后续就不再提供通知,程序员自己要再次调用EPOLL_CTL_MOD来启用通知.

如何验证这两种触发模式?
(水平/边缘触发)x(阻塞/非阻塞)x(server fd/client fd)
https://blog.csdn.net/liu0808/article/details/52980413

主要是验证,
边缘触发下,一次没读完,后续消息不通知的情形
水平触发下,一直报告epollout事件的情形


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

### (select/poll/epoll)这些io多路复用是怎么实现的
https://www.zhihu.com/question/28594409/answer/295638973


select是怎么实现的



epoll是怎么实现的


使用epoll要先通过epoll_create创建一个epfd
这个epfd对应着系统内核中的以文件
它有一个结构体`struct eventpoll`,这个结构体
记录了用户通过epoll_ctl调用放进来的socket fd
记录了调用它的进程列表(一个epfd可以被多个进程epoll_wait)
记录了注册的socket fd中活跃的部分


https://medium.com/@copyconstruct/the-method-to-epolls-madness-d9d2d6378642
https://idea.popcount.org/2017-02-20-epoll-is-fundamentally-broken-12/

### select/poll/epoll之间的比较
https://www.cnblogs.com/aspirant/p/9166944.html
socket有一个等待列表
对于select这种方式
每次调用,都要讲进程放入到socket的等待列表
调用结束的时候,把进程从socket的等待列表中取出来
程序员要再次遍历fdset,用fdisset来判断socket是否可以操作了

每次调用select，都需要把fd集合从用户态拷贝到内核态，这个开销在fd很多时会很大（因为在内核才能监听那些数据，也就是操作文件描述符的读写）

同时每次调用select都需要在内核遍历传递进来的所有fd，这个开销在fd很多时也很大

select支持的文件描述符数量太小了，默认是1024

 epoll既然是对select和pol的改进,就应该能避免上述的三个缺点。那epoll都是怎么解决的呢?在此之前,我们先看一下epoll和select和poll的调用接口上的不同, select和poll都只提供了一个函数select或者poll函数。 而epoll提供了三个函数, epoll create,epoll cti和epoll wait , epoll create是创建一个epol句柄 ; epoll ctl是注册要监听的事件类型; epoll wait则是等待事件的产生。

对于第一-个缺点, epoll的解决方案在epoll ctl函数中。每次注册新的事件到epoll句柄中时(在epoll ctI中指定EPOLL CTL ADD) ,会把所有的fd拷贝进内核,而不是在epoll wait的时候重复拷贝。epoll保证 了每个fd在整个过程中只会拷贝一次。

对于第二个缺点, epoll的解决方案不像select或poll- -样每次都把current轮流加入fd对应的设备等待队列中,而只在epoll ctl时把current挂一遍(这一遍必不可少)并为每个fd指定一-个回调函数 ,当设备就绪,唤醒等待队列上的等待者时,就会调用这个回调函数,而这个回调函数会把就绪的fd加入-一个就绪链表)。epoll wait的工作实际上就是在这个就绪链表中查看有没有就绪的fd (利用schedule_ timeout0实现睡一会,判断一会的效果 ,和select实现中的第7步是类似的)。

对于第三个缺点, epoll没有这个限制，它所支持的FD上限是最大可以打开文件的数目, 这个数字-般远大于2048,举个例子，在1GB内存的机器上大约是10万左右,具体数目可以cat /proc/sys/fs/file-max察看,一般来说这个数品和系统内存关系很大。


epoll和select/poll不一样
这一点在操作使用流程上就能体现出来
使用select/poll,每次都要
使用epoll,只在epoll_create的时候操作相关的等待队列

epoll额外创建了一个文件描述符,efd,然后把epf放入到socket的等待列表
把进程放入到efd的等待列表
即在进程和进程等待的socket fd之间加了一层
然后epoll_wait调用返回的时候,直接返回可用的socket fd


epoll大体实现


但是,epoll比select/poll更快这一点并不是绝对的


### FAQ
socket low-water mark
What's the purpose of the socket option SO_SNDLOWAT
https://stackoverflow.com/questions/8245937/whats-the-purpose-of-the-socket-option-so-sndlowat


怎样用非阻塞socket 发生文件等大数据量的消息
即非阻塞socket怎么处理一次发不完的数据?
nonblock socket send file
非阻塞 大量数据传输

https://bbs.csdn.net/topics/380167545

非阻塞 大数据量

https://www.cnblogs.com/xiohao/p/4385508.html


http://www.169it.com/tech-qa-linux/article-12711598179030876197.html

socket send big data

https://stackoverflow.com/questions/12912599/sending-large-data-via-socket

https://stackoverflow.com/questions/1577825/unix-sockets-how-to-send-really-big-data-with-one-send-call?rq=1

nonblock socket send large data

https://stackoverflow.com/questions/39991227/tcp-socket-multiplexing-send-large-data


socket 如何优雅关闭
shutdown vs close
graceful shutdown socket
产生了大量的time_wait/close_wait ,怎么办?怎么发现?
