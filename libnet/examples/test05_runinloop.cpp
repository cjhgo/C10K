#include "libnet/libnet.hpp"


using namespace libnet;

EventLoop* g_loop;
int g_flag =0;

void run4()
{
  printf("run4(): \n");
  g_loop->quit();
}


void run3()
{
  printf("run3():\n");
  g_loop->runAfter(3, run4);
  g_flag = 0;
}

void run2()
{
  printf("run2():\n");
  g_loop->queueInLoop(run3);
  g_flag=2;
}


void run1()
{
  g_flag = 1;
  printf("run1():\n");
  g_loop->runInLoop(run2);
  g_flag=2;
}

int main(int argc, char const *argv[])
{
  EventLoop loop;
  g_loop = &loop;
  loop.runAfter(2, run1);
  loop.loop();
  printf("main()end\n");
  return 0;
}
