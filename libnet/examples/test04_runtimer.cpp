#include "libnet/libnet.hpp"
#include <stdio.h>

using namespace libnet;


int cnt = 0;
EventLoop* g_loop;


void use_timestamp()
{
  Timestamp now = Timestamp::now();
  printf("%s\t%s\n", now.toString().c_str(), now.toFormattedString().c_str());
}

void print(const char* msg)
{
  printf("msg %s %s\n", "", msg);
  if(cnt++ > 20)
  {
    g_loop->quit();
  }
}

int main(int argc, char const *argv[])
{
  Logger::instance().setLogLevel(NONE);
  EventLoop loop;
  g_loop = &loop;

  printf("main begin\n");
  use_timestamp();
  loop.runAt(addTime(Timestamp::now(),5), std::bind(print, "once1"));
  loop.runAfter(1, std::bind(print, "once1"));
  loop.runAfter(1.5, std::bind(print, "once1.5"));
  loop.runAfter(2.5, std::bind(print, "once2.5"));
  loop.runAfter(3.5, std::bind(print, "once3.5"));
  loop.runEvery(2, std::bind(print, "every2"));
  loop.runEvery(3, std::bind(print, "every3"));

  loop.loop();
  print("main loop quit\n")  ;

  return 0;
}
