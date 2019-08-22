#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "libnet/net/Callbacks.h"
#include "libnet/base/Timestamp.h"
#include "Channel.h"
#include <vector>
#include <set>

namespace libnet
{

class EventLoop;
class Timer;
class TimerId;


class TimerQueue
{
 public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

  // void addTimer(Timer* t);

 private:
  typedef std::pair<Timestamp,Timer*> Entry;
  typedef std::set<Entry> TimerList;


  void addTimerInLoop(Timer* timer);
  void handleRead();

  std::vector<Entry> getExpired(Timestamp now);
  void reset(const std::vector<Entry>& expired, Timestamp now);

  bool insert(Timer* timer);

  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  TimerList timers_;

};

}

#endif
