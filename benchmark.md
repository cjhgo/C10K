[toc]


怎么做benchmark?


相关的搜索关键词
```
echo 性能测试
网络库 压力测试
socket qps 测试
```

### 有哪些benchmark工具可以使用?

https://gist.github.com/denji/8333630
http-benchmark.md
这里列出了各种各样的benchmark工具

https://github.com/tsenart/vegeta

https://github.com/parkghost/gohttpbench
An ab-like benchmark tool run on multi-core cpu
(因为ab是单线程的?这个可以指定使用的cpu数目?)

wrk2


这里要区分,ab是对web http进行压力测试的
即接口必须走的是http协议

但显然有的时候,我们需要的是对tcp socket的压力测试
这时ab就用不了了,有哪些工具可以使用呢
好像jmeter可以
https://jmeter.apache.org/
基于tcp/udp socket多线程并发实现对二进制协议的压力测试
https://blog.csdn.net/kkkkkkkkkkkk_/article/details/17755643


专门用于tcp benchmark的工具好像也有很多
比如,iperf,uperf...


为什么需要压力测试工具呢?
因为我们需要很方便的按照指定的参数(并发级别,线程数)来启动相应的并发请求场景
虽然也可以自己编程实现,但是用ab,wrk2,gohttpbecn这样的工具会简单方便一些.

### 对网络库做benchmark的思路/原理是什么?
如何对网络库（例如 muduo 库）做抗压测试？
https://www.zhihu.com/question/271188675


哪些因素影响网络库的性能?

1. 硬件资源:40核的机器肯定要比4核的机器处理能力强(代码充分利用了硬件资源的条件下)
2. 代码的质量
加锁的粒度,有没有阻塞的调用,没必要的输入输出
3. 请求的类型
简单的echo请求
质数分解的请求(计算密集)
到数据库查数据的请求(io密集)
不同的请求需要的处理时间不一样,相应的处理延迟不一样
ab测试工具
4. 程序的线程数目



哪些指标反应网络库的性能?

指定了c和n参数之后的


1. cpu占用率
2. 内存占用率
3. 请求处理的延迟
4. 整体的吞吐量
5. QPS(每秒响应多少请求)


所以,压力测试工具做了什么事情?

按照c的并发等级对指定的网络地址发送n个网络请求
然后统计
`平均处理延迟/整体吞吐量/QPS`

### 压力测试的目的是什么?

### 怎样用ab进行benchmark
ab是一个单线程的测试工具

由于主流的benchmark工具都是面向web的,即发送的是http报文
所以,这里实现的虽然是echo socket,但是
在做出响应的时候,也还是要返回http格式的返回才能和ab测试工具兼容
一个最简单的http响应报文格式如下
```
HTTP/1.1 200 OK

echo
```
用编程的字符串来表示的话是
`HTTP/1.1 200 OK\r\n\r\necho`
即状态码+空白行+bodymessage
更加全面的关于http协议的介绍见
https://www.w3.org/Protocols/rfc2616/rfc2616.html
https://www.tutorialspoint.com/http/http_responses.htm
这些个页面



还有一点,用ab进行测试的时候` ab -c 5 -n 1000  http://127.0.0.1:8899/`
最后这个`/`千万不能少,否则ab报错`invalid url`

怎么理解ab测试的结果,见博文


其他人做的压力测试实践

+ C++网络通信库性能大比拼
作者使用wrk对cppcms, boost asio, libevent, muduo和 nginx进行了测试
使用的命令是这样的
`wrk -t4 -c200 -d60s "http://167serverip:serverport/collect?v=1"`
4线程,200并发等级,持续60s
测试环境:`Xeon@2.4G+8core+16G`统计了qps指标,muduo的值是300k
https://blog.csdn.net/csfreebird/article/details/49283743
+ 网络库压力测试：mongols VS evpp
作者对360开发的evpp进行了压力测试,开4个线程,qps大概为30k
  ```
  ab -kc100 -n100000 http://127.0.0.1:29099/
  ab -kc500 -n100000 http://127.0.0.1:29099/
  ab -kc800 -n100000 http://127.0.0.1:29099/
  ab -kc1000 -n100000 http://127.0.0.1:29099/
  ```
  https://www.cnblogs.com/hi-nginx/p/9527696.html

所以,一般来说,echo 测试的qps至少得10k才拿得出手吧...

`并发50000个线程时，qps可以达到1.6w左右（服务器时延在10ms以下）`