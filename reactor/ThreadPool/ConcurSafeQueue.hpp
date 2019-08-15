#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ConcurSafeQueue
{

public:
  void push(T e)
  {
    std::lock_guard<std::mutex> lock(mut);
    data.push(e);
    condiation.notify_one();
  }
  bool empty()
  {
    std::lock_guard<std::mutex> lock(mut);
    return data.empty();
  }
  void wait_and_pop(T& e)
  {
    
    std::unique_lock<std::mutex> lock(mut);
    if( data.empty())
    {
      condiation.wait(lock,
          [&]
          {
            return !data.empty();
          });     
    }
    e=std::move(data.front());
    data.pop();
  }
private:
  std::queue<T> data;
  std::mutex mut;
  std::condition_variable condiation;
};