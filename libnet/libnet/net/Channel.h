#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include "libnet/base/Timestamp.h"

namespace libnet
{
class EventLoop;

class Channel
{
 public:
  typedef std::function<void()> EventCallback;
  typedef std::function<void(Timestamp)> ReadEventCallback;

  Channel(EventLoop* loop, int fd);
  ~Channel();
  
  void handleEvent(Timestamp reveiveTime);

  void setReadCallback(const ReadEventCallback& cb)
  { readCallback = cb;}
  void setWriteCallback(const EventCallback& cb)
  { writeCallback = cb;}
  void setErrorCallback(const EventCallback& cb)
  { errorCallback = cb;}
  void setCloseCallback(const EventCallback& cb)
  { closeCallback = cb;}

  int fd()const {return fd_;}
  int events()const{return events_;}
  void set_revents(int revt){revents_ = revt;}
  bool isNoneEvent() const { return events_ == kNoneEvent;}

  void enableReading()
  {
    events_ |= kReadEvent; 
    update();
  }

  void enableWriting()
  {
    events_ |= kWriteEvent;
    update();
  }

  void disableWriting()
  {
    events_ &= ~kWriteEvent;
    update();
  }

  void disableAll()
  {
    events_ = kNoneEvent;
    update();
  }

  //要给对方发消息,关心此fd的可写事件
  bool isWriting() const
  {
    return events_ & kWriteEvent;
  }  
  int index() {return index_;}
  void set_index(int idx) { index_ = idx;}

  EventLoop* ownerLoop() { return loop_; }
 private:
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  void update();
  EventLoop* loop_;
  const int fd_;
  int events_;//记录用户在此fd上关心的事件,bit标志位
  int revents_;//记录poll返回的实际发生的事件,bit标志位
  int index_;//记录本channel在poll中的pfds中的位置下标

  bool eventHandling_;
  ReadEventCallback readCallback;
  EventCallback writeCallback;
  EventCallback errorCallback;
  EventCallback closeCallback;
  
};
}
#endif