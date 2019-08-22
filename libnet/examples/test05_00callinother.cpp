#include "libnet/libnet.hpp"
#include <thread>

using namespace libnet;

EventLoop* g_loop;


void foo()
{}


void threadFunc()
{
  g_loop->runAfter(1.0,foo);
}



int main(int argc, char const *argv[])
{
  EventLoop loop;
  g_loop = &loop;
  std::thread a(threadFunc);  
  loop.loop();
  printf("main()end\n");
  return 0;
}
