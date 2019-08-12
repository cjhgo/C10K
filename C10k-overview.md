## C10K

### 从优秀的开源库中学习:libevent,libuv,redis,tornado
libevent , 异步io?

libevent提供了一个跨平台可移植的轮询操作api.
select太低效,linux上有epoll,BSD上有kqueue.

A tiny introduction to asynchronous IO
http://www.wangafu.net/~nickm/libevent-book/01_intro.html
http://www.wangafu.net/~nickm/libevent-2.1/doxygen/html/


An Introduction to libuv
https://nikhilm.github.io/uvbook/An%20Introduction%20to%20libuv.pdf

学习tornado的ioloop和对poll api接口的封装
