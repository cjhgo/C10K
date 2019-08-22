#ifndef TIMERID_H
#define TIMERID_H

namespace libnet
{
class Timer;

class TimerId
{
 public:
  explicit TimerId(Timer* timer)
    : value_(timer)
  {}
 private:
  Timer* value_;
};

}

#endif
