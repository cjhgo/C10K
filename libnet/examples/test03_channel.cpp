#include <sys/timerfd.h>
#include "libnet/libnet.hpp"
#include <string.h>

using namespace libnet;


EventLoop* g_loop;

void timeout(Timestamp receiveTime)
{
  printf("%s Timeout!\n", receiveTime.toFormattedString().c_str());
  g_loop->quit();
}

int main(int argc, char const *argv[])
{
  Logger::instance().setLogLevel(NONE);
  EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
  Channel channel(g_loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  
  handleErr(
    ::timerfd_settime(timerfd, 0, &howlong, NULL), 
    "set time error:");

  loop.loop();

  ::close(timerfd);
  return 0;
}
