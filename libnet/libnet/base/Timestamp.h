#ifndef TIMESTAMP_H
#define TIMESTAMP_H


#include <stdint.h>
#include <string>

namespace libnet
{

//微妙粒度的时间戳类,1s=1000*1000微秒
//秒s,毫秒millisecond,微秒microsecond
class Timestamp
{
 public:  
  Timestamp();
  explicit Timestamp(uint64_t microseconds);

  void swap(Timestamp& that)
  {
    std::swap(sinceEpoch_, that.sinceEpoch_);
  }

  uint64_t sinceEpoch() const 
  {
    return sinceEpoch_;
  }

  std::string toString() const;
  std::string toFormattedString() const;

  bool valid() const { return sinceEpoch_ > 0;}


  static Timestamp invalid();
  static Timestamp now();

  static const int kMicroSecondsPerSecond = 1000 * 1000;//s和us之间的换算
 private:
  uint64_t sinceEpoch_;//自Epoch算起,以us为单位的时间戳
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
  return lhs.sinceEpoch() < rhs.sinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.sinceEpoch() == rhs.sinceEpoch();
}


inline double timeDifference(Timestamp high, Timestamp low)
{
  int64_t diff = high.sinceEpoch() - low.sinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}


inline Timestamp addTime(Timestamp timestamp, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.sinceEpoch() + delta);
}

}
#endif