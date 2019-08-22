#ifndef TIMER_H
#define TIMER_H

#include "libnet/base/Timestamp.h"
#include "libnet/net/Callbacks.h"

namespace libnet
{

class Timer
{
 public:

  Timer(const TimerCallback& cb, Timestamp when,  double interval)
    : timercb_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0)
  {}

  void run()
  {
    timercb_();
  }

  Timestamp expiration()const
  {
    return expiration_;
  }

  bool repeat() const
  {
    return repeat_;
  }

  void restart(Timestamp now)
  {
    if(repeat_)
    {
      expiration_ = addTime(now, interval_);
    }else
    {
      expiration_ = Timestamp();
    }
  }

 private:
  
  TimerCallback timercb_;
  Timestamp expiration_;
  const double interval_;
  const bool repeat_;
};

}
#endif