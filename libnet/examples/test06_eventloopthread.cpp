#include "libnet/libnet.hpp"
using namespace libnet;

void runInThread()
{
  printf("runinthread(): \n");
}


int main(int argc, char const *argv[])
{
  printf("main():\n");

  EventLoopThread loopthread;
  EventLoop* loop = loopthread.startLoop();
  loop->runInLoop(runInThread);
  sleep(1);
  loop->runAfter(2, runInThread);
  sleep(3);
  // loop->quit();
  printf("main()exit\n");
  return 0;
}
