#include "ThreadPool.hpp"
#include "ConcurSafeQueue.hpp"
#include "queue.hpp"
#include <thread>
#include <chrono>
#include <cstdio>
#include <iostream>
void long_task()
{
  std::cout<<"iiii\n";
  // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout<<"jjjj\n";
}
typedef void (*FunType)(void);
struct Task{
  Task(){};
  Task(FunType fun, int id=0)
  :fun(fun),taskid(id){};
  FunType fun;
  int taskid;
};

typedef ThreadPool<ConcurSafeQueue,Task> ThreadPool_Type;
// typedef ThreadPool<threadsafe_queue,Task> ThreadPool_Type;
int main(int argc, char const *argv[])
{
  ThreadPool_Type tp(std::thread::hardware_concurrency());
  for(size_t i = 0; i < 10000; i++)
  {
      tp.add(Task(long_task,i));
  }  
  tp.run();
  return 0;
}