#include "libnet/net/EventLoopThreadPool.h"
#include "libnet/net/EventLoop.h"
#include "libnet/net/EventLoopThread.h"
#include <assert.h>

using namespace libnet;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  : baseLoop_(baseLoop),
    started_(false),
    numThreads_(0),
    next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}


void EventLoopThreadPool::start()
{
  assert(!started_);
  baseLoop_->assertInLoopThread();
  started_ = true;
  for(int i = 0; i < numThreads_; i++)
  {
    EventLoopThread* t = new EventLoopThread;
    threads_.push_back(t);
    loops_.push_back(t->startLoop());
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  EventLoop* loop = baseLoop_;
  if(not loops_.empty())
  {
    loop = loops_[next_];
    assert(numThreads_ > 0);
    next_ = (next_+1)%numThreads_;
  }
  return loop;
}

