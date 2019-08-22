#include <stdio.h>
#include "libnet/libnet.hpp"

using namespace libnet;

EventLoop* g_loop;
void threadFunc2()
{
  g_loop->loop();
}
int main(int argc, char const *argv[])
{
  ::printf("threadfunc(): tid %d\n", std::this_thread::get_id());
  EventLoop loop;
  g_loop = &loop;
  std::thread a(threadFunc2);
  a.join();
  return 0;
}
