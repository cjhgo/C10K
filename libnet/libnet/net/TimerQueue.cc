#include "TimerQueue.h"
#include "TimerId.h"
#include "Timer.h"
#include "EventLoop.h"
#include "utils/SocketsOps.h"
#include "libnet/base/logger.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace libnet
{

//创建一个timerfd
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
  handleErr(timerfd, "create timerfd error!\t");
  return timerfd;
}

//把when-now之间的时间差以timespec的格式返回
struct timespec howMuchTimeFromNow(Timestamp when)
{
  uint64_t microseconds = when.sinceEpoch() - Timestamp::now().sinceEpoch();
  if(microseconds < 100)
  {
    microseconds = 100;
  }
  
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microseconds/Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
          (microseconds %Timestamp::kMicroSecondsPerSecond)*1000);
  
  return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  if( n != sizeof howmany)
  {
    LOG_DEBUG<<"read from timer fd error! "<<n<<"\n";
  }

}


//设置timerfd的过期时间,一个timerfd可以被多次反复设置过期时间
void resetTimerfd(int timerfd, Timestamp expiration)
{
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);//必须有这一步
  bzero(&oldValue, sizeof oldValue);//必须有这一步
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  handleErr(ret, "set timer fd set error!\t");

}

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop,timerfd_),
    timers_()
{
  timerfdChannel_.setReadCallback(
    std::bind(&TimerQueue::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  sockets::close(timerfd_);
  for(TimerList::iterator it = timers_.begin();
        it != timers_.end(); it++)
  {
    delete it->second;
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  bool earliestChanged = insert(timer);
  if(earliestChanged)
  {
    resetTimerfd(timerfd_, timer->expiration());
  }
}

void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);
  std::vector<Entry> expired = getExpired(now);
  for(std::vector<Entry>::iterator it = expired.begin();
        it!=expired.end(); ++it)
  {
    it->second->run();
  }
  reset(expired, now);
}


bool TimerQueue::insert(Timer* timer)
{
  bool earliesChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if( it == timers_.end() || when < it->first)
  {
    earliesChanged = true;
  }
  std::pair<TimerList::iterator, bool> result =
      timers_.insert({when, timer});
  assert(result.second);
  return earliesChanged;
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  std::vector<Entry> expired;
  Entry sentry = {now, reinterpret_cast<Timer*>(UINTPTR_MAX)};
  auto it = timers_.lower_bound(sentry);
  assert(it == timers_.end() || now < it->first);
  std::copy(timers_.begin(), it, std::back_inserter(expired));
  timers_.erase(timers_.begin(), it);
  return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;
  for(auto it = expired.begin(); it != expired.end(); it++)
  {
    if(it->second->repeat())
    {
      it->second->restart(now);
      insert(it->second);
    }
    else
    {
      delete it->second;
    }
  }
  if(!timers_.empty() && (timers_.begin()->first).valid())
  {
    resetTimerfd(timerfd_, timers_.begin()->second->expiration());
  }
}


}



