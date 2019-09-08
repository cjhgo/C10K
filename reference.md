### 参考资料


unp太老了,没提到epoll,讲的又是unix平台上的事情,linux还是有不一样的地方的 \
像*epoll,socket resueaddr,惊群,reactor模型,tcp的time_wait...* \
这些编程实际用到的,知识,就没有哪本书系统的讲解一下吗 \
而不是看哪些没有经过审核的,什么人都能写的,错误百出,未经验证的博客? \

陈硕的linux多线程服务端编程更像是自己职业生涯感悟的一个杂集 \
并没较多的覆盖这些问题

unp,tcp/ip-illust可以作为手册来查询 \
这里推荐两本书,可以作为业界的经验实践总结

国人写的一本- Linux高性能服务器编程 \
外国人写的一本- Linux system programming


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
