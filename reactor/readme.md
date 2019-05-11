

### 基于reactor模型实现一个简单的echo server

主线程不断检查可读事件,子线程处理可读事件.
可读事件有两种,server socket可以accept, client socket可以read.

1. create/bind/listen server socket, 创建server socket`server_fd`
2. epoll_create 创建epoll 实例
3. 向epoll实例注册server_fd的可读事件
4. 启动线程池不断从fd_queue中取可读fd
    1. 如果取到`fd==server_fd`,则说明有新的链接进来,执行`client=accept4`,
    向epoll实例注册这个client的边缘触发可读事件
    2. 取到的不是server_fd,则说明clinet发来了数据,读数据然后echo
5. 启动主线程的io-loop,用epoll_wait不断检查是否有可读事件发生
