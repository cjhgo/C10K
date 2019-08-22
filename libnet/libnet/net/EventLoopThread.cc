#include "libnet/net/EventLoopThread.h"
#include "libnet/net/EventLoop.h"

using namespace libnet;



EventLoopThread::EventLoopThread()
  : loop_(NULL),
    existing_(false),    
    mutex_(),
    cond_()
{

}


EventLoopThread::~EventLoopThread()
{
  existing_=true;
  loop_->quit();
  thread_.join();
}


EventLoop* EventLoopThread::startLoop()
{
  thread_=std::thread(std::bind(&EventLoopThread::threadFunc, this));
  {
    std::unique_lock<std::mutex> lock(this->mutex_);
    while (loop_ == NULL)
    {
      cond_.wait(lock, [&]{
          return loop_ != NULL;
      });
    }    
  }
  return loop_;

}


void EventLoopThread::threadFunc()
{
  EventLoop loop;
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.loop();
}