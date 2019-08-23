#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <map>

#include "libnet/net/EventLoop.h"
#include "libnet/net/Channel.h"

// struct polllfd;
typedef std::vector<struct pollfd> PollFdList;
namespace libnet
{
class Poller
{
 public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);
  ~Poller();

  //用activeChannels接收poll的结果
  //sys/poll的风格是,sys/epoll的风格是
  //都可以用?这样接收?
  void poll(int timeoutMs, ChannelList* activeChannels);

  //通过channel注册(fd,events)
  void updateChannel(Channel* channel);

  void removeChannel(Channel* channel);

  void assertInLoopThread() {ownerLoop_->assertInLoopThread();}
 private:
  //把poll调用对pollfds_的修改填充到activechannels中
  void fillActiveChannel(int numEvents, 
                          ChannelList* activeChannels) const;

  
  typedef std::map<int, Channel*> ChannelMap;

  EventLoop* ownerLoop_;
  PollFdList pollfds_;//记录注册的fd set
  ChannelMap channels_;//记录fd对应的Channel*
};
}
#endif