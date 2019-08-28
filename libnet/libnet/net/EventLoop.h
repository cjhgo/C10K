#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include "TimerId.h"
#include "libnet/base/Timestamp.h"
#include "libnet/net/Callbacks.h"
#include "libnet/base/logger.h"

namespace libnet
{
class Channel;
class Poller;
class TimerQueue;
class EventLoop
{
 public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();

  EventLoop(EventLoop&)=delete;
  EventLoop(EventLoop&&)=delete;
  EventLoop& operator=(EventLoop&)=delete;
  
  EventLoop* getEventLoopOfCurrentThread();



  void loop();
  void quit();



  void runInLoop(const Functor& cb);

  void queueInLoop(const Functor& cb);

  TimerId runAt(const Timestamp& time, const TimerCallback& cb);

  TimerId runAfter(const double delay, const TimerCallback& cb);

  TimerId runEvery(double interval, const TimerCallback& cb);

  void wakeup();

  void updateChannel(Channel* channel);

  void removeChannel(Channel* channel);

  bool isInLoopThread() const
  {    
    return threadId_ == std::this_thread::get_id();
  }

  void assertInLoopThread()
  {
    if(! isInLoopThread())
    {
      abortNotInLoopThread();
    }
  }
 private:


  
  void abortNotInLoopThread();
  bool looping_; //处于ioloop
  bool quit_; //已经停止ioloop

  
  std::shared_ptr<Poller> poller_;
  std::shared_ptr<TimerQueue> timerQueue_;
  
  typedef std::vector<Channel*> ChannelList;
  ChannelList activeChannels_;
  const std::thread::id threadId_;// = std::this_thread::get_id();
  

  int wakeupFd_;
  bool callingPendingFunctors_;
  std::shared_ptr<Channel> wakeupChanel_;
  std::mutex mutex_;
  std::vector<Functor> pendingFunctors_;
  void handleRead();
  void doPendingFunctors();
};
}
#endif