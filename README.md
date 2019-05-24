[toc]

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



学习redis的ioloop和对poll api接口的封装

### 异步编程&异步io
回调风格的异步编程如何实现
协程如何取代callback
```py
from tornado.concurrent import Future

@gen.coroutine
def async_fetch_gen(url):
    http_client = AsyncHTTPClient()
    response = yield http_client.fetch(url)
    raise gen.Return(response.body)


def async_fetch_manual(url):
    http_client = AsyncHTTPClient()
    my_future = Future()
    fetch_future = http_client.fetch(url)
    def on_fetch(f):
        my_future.set_result(f.result().body)
    fetch_future.add_done_callback(on_fetch)
    return my_future
```
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


面向模式的软件体系结构 posa 系列 是一本高阶的参考资料


有什么适合提高 C/C++ 网络编程能力的开源项目推荐？
https://www.zhihu.com/question/20124494

操作系统缓冲区

when send block
operating system buffer nic
操作系统 缓冲区
operating system buffer
socket buffer

https://stackoverflow.com/questions/30429002/when-would-a-blocking-socket-send-block-udp
https://eklitzke.org/how-tcp-sockets-work
