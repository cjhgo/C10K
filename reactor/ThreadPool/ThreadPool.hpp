#include <iostream>
#include <thread>
#include <cstdio>
#include <vector>

template< template<class> class QueueType, class TaskType>
class ThreadPool
{
public:
  
  //void launch();
  void run();
  void join();
  void worker();
  void add(TaskType t);
  ThreadPool(std::size_t thread_cnt)
  :thread_cnt(thread_cnt),running(true)
  {};
  ~ThreadPool();
private:
  bool running;
  size_t thread_cnt;
  std::vector<std::thread> pool;
  QueueType<TaskType> task_queue;
};


template< template<class> class QueueType, class TaskType>
void ThreadPool<QueueType,TaskType>::worker()
{
  
  while (running)
  {
    // std::cout<<"worker begin\n";
    TaskType t;
    // std::cout<<"wait for task...\n";
    this->task_queue.wait_and_pop(t);
    // std::cout<<"worker get\n";
    t.fun();
    
  }  
}


template< template<class> class QueueType, class TaskType>
void ThreadPool<QueueType, TaskType>::run()
{
  for(std::size_t i = 0; i < thread_cnt; i++)
  {
    pool.emplace_back([this] { worker(); });
  }
}

template< template<class> class QueueType, class TaskType>
void ThreadPool<QueueType, TaskType>::join()
{
  for(auto& thread : pool)
  thread.join();
}


template< template<class> class QueueType, class TaskType>
void ThreadPool<QueueType,TaskType>::add(TaskType t)
{
  // std::cout<<"run to here\n";
  this->task_queue.push(t);
}
template< template<class> class QueueType, class TaskType>
ThreadPool<QueueType,TaskType>::~ThreadPool<QueueType,TaskType>()
{
  
}