#include <cassert>
#include <sys/eventfd.h>

#include "libnet/net/EventLoop.h"
#include "libnet/net/Poller.h"
#include "libnet/base/logger.h"
#include "libnet/net/TimerQueue.h"
#include "libnet/net/utils/SocketsOps.h"

using namespace libnet;



static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);

  handleErr(evtfd, "");

  return evtfd;
}

thread_local EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 100;

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    threadId_(std::this_thread::get_id()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),

    wakeupFd_(createEventfd()),
    wakeupChanel_(new Channel(this, wakeupFd_))
{
  if( t_loopInThisThread == 0)
  {
    t_loopInThisThread = this;
  }
  else
  {
    //
  }
  wakeupChanel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChanel_->enableReading();
  
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  t_loopInThisThread = nullptr;
}


EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_=true;
  quit_=false;

  while (!quit_)
  {
    activeChannels_.clear();
    poller_->poll(kPollTimeMs, &activeChannels_);
    for(ChannelList::iterator it = activeChannels_.begin();
          it!=activeChannels_.end(); it++)
    {
      (*it)->handleEvent();
    }
    doPendingFunctors();
  }
  looping_=false;

}

void EventLoop::quit()
{
  quit_ = true;
  if(!isInLoopThread())
  {
    wakeup();
  }
}

void EventLoop::runInLoop(const Functor& cb)
{
  if(isInLoopThread())
  {
    cb();
  }
  else
  {
    queueInLoop(cb);
  }
}


void EventLoop::queueInLoop(const Functor& cb)
{
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    pendingFunctors_.push_back(cb);
  }
  if( !isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();
  }
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{

  return timerQueue_->addTimer(cb, time, 0.0);

}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
  Timestamp time(addTime(Timestamp::now(), delay));
  return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
  Timestamp time(addTime(Timestamp::now(), interval));
  return timerQueue_->addTimer(cb, time, interval);
}


void EventLoop::abortNotInLoopThread()
{
  LOG_DEBUG<<"not in loop thread, exit now ...\n";
  // exit(2);
  abort();
}


void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one);
  handleErr(n);
}


void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one);
  handleErr(n);
}


void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    functors.swap(this->pendingFunctors_);
  }
  for(auto& it : functors)
  {
    it();
  }

  callingPendingFunctors_=false;
}

void EventLoop::updateChannel(Channel* channel)
{
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->removeChannel(channel);
}