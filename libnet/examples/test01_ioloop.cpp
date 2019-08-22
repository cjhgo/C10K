#include <stdio.h>
#include "libnet/libnet.hpp"

using namespace libnet;



void threadFunc()
{
  ::printf("threadfunc(): tid %d\n", std::this_thread::get_id());
  EventLoop loop;
  loop.loop();
}


int main(int argc, char const *argv[])
{
  ::printf("threadfunc(): tid %d\n", std::this_thread::get_id());
  EventLoop loop;
  
  std::thread a(threadFunc);
  loop.loop();
  a.join();

  return 0;
}
