#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

namespace libnet
{


class EventLoop;

class EventLoopThread
{
 public:
  EventLoopThread();
  ~EventLoopThread();
  EventLoop* startLoop();


 private:
  void threadFunc();

  EventLoop* loop_;
  bool existing_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
};
}

#endif