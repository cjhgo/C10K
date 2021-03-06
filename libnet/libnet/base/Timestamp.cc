#include "Timestamp.h"
#include <sys/time.h>

using namespace libnet;

Timestamp::Timestamp()
  : sinceEpoch_(0)
{

}


Timestamp::Timestamp(uint64_t microseconds)
  : sinceEpoch_(microseconds)
{

}


std::string Timestamp::toString() const
{
  char buf[32]={0};
  uint64_t seconds = sinceEpoch_ / kMicroSecondsPerSecond;
  uint64_t microseconds = sinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%d.%06d", seconds, microseconds);
  return buf;
}

std::string Timestamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(sinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(sinceEpoch_ % kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
  return Timestamp();
}