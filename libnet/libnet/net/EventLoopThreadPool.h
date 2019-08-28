#include <vector>

namespace libnet
{

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool
{

 public:
  EventLoopThreadPool(EventLoop* baseLoop);
  ~EventLoopThreadPool();
  EventLoop* getNextLoop();
  void start();
  void setThreadNum(int numThreads){numThreads_ = numThreads;}
 private:
  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<EventLoopThread*> threads_;
  std::vector<EventLoop*> loops_;

};
}