#include "libnet/net/EventLoopThread.h"
#include "libnet/net/EventLoop.h"
#include "libnet/base/logger.h"

using namespace libnet;



EventLoopThread::EventLoopThread()
  : loop_(NULL),
    exiting_(false),    
    mutex_(),
    cond_()
{

}


EventLoopThread::~EventLoopThread()
{
  exiting_=true;  
  
  thread_.join();
  loop_->quit();
  
}


EventLoop* EventLoopThread::startLoop()
{
  thread_=std::thread(std::bind(&EventLoopThread::threadFunc, this));
  LOG_DEBUG<<"threadfunc begin...\n";
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
  LOG_DEBUG<<"loop ending...?\n";
}