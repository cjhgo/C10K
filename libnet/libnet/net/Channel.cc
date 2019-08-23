#include "libnet/net/Channel.h"
#include "libnet/net/EventLoop.h"
#include "libnet/base/logger.h"
#include <poll.h>
#include <assert.h>

using namespace libnet;
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;



Channel::Channel(EventLoop* loop, int fdArg)
  :loop_(loop),fd_(fdArg),events_(0),revents_(0),index_(-1),
  eventHandling_(false)
{

}

Channel::~Channel()
{
  assert(!eventHandling_);
}

void Channel::update()
{
  loop_->updateChannel(this);
}


void Channel::handleEvent()
{
  eventHandling_ = true;
  if(revents_ &(POLLHUP) && !(revents_ & POLLIN))
  {
    LOG_DEBUG<<"close events on fd"<<this->fd_<<"\n";
    if(closeCallback)
    {
      closeCallback();
    }
  }
  if(revents_ & (POLLERR | POLLNVAL))
  {
    LOG_DEBUG<<"error events on fd "<<this->fd_<<"\n";
    if(errorCallback)errorCallback();
  }
  if( revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    LOG_DEBUG<<"in events on fd "<<this->fd_<<"\n";
    if(readCallback)readCallback();
  }
  if( revents_ & ( POLLOUT))
  {
    LOG_DEBUG<<"out events on fd "<<this->fd_<<"\n";
    if(writeCallback) writeCallback();
  }
  eventHandling_=false;
}