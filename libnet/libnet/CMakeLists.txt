

add_library(libnet 
  base/Timestamp.cc
  net/Acceptor.cc
  net/Channel.cc
  net/EventLoop.cc
  net/EventLoopThread.cc
  net/poller
  net/Poller.cc
  net/TcpConnection.cc
  net/TcpServer.cc
  net/TimerQueue.cc
  net/utils/InetAddress.cc
  net/utils/Socket.cc
  net/utils/SocketOps.cc  
)
target_link_libraries(libnet pthread)