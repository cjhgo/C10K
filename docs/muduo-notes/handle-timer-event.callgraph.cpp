/**
 * 定时器的处理逻辑
 */


/** 定时器的注册 */

EventLoop loop
{//构造函数
  timerQueue(new TimerQueue(this))
  [//TimerQueue::TimerQueue(EventLoop*)
    timerfd = createtimerfd()
    timerfdchannel = Channel(loop,timerfd)
    timers = set< <Timestamp,Timer*> >
    timerfdchannel.setreadcb(TimerQueue::handleRead)
    timerfdchannel.enableReading()
  ]
}


/**用户添加一个定时事件 */

timerqueue->
loop.runAt(cb,when)
{//EventLoop::runAt(Timestamp&, TimerCallback&)
  timerqueue->addTimer(cb,time,0)
  [//TimerQueue::addTimer(TimerCallback&,Timestamp,double)
    Timer* timer = new Timer(cb,when,interval)
    loop->runinloop(TimerQueue::addTimerInLoop)
    [//TimerQueue::addTimerInLoop(Timer*)
      earliest = insert(timer)
      [//TimerQueue::insert(Timer*)
        如果timers是空的,或我的过期时间比timers中的第一个早
        那么,我是earliest
        timers.insert({when,timer})
      ]
      if earlies
      {
        如果新插入的timer是最早的,按它的时间来激活timerfd
        resetTimerfd(timerfd,timer->expiration)
        [//::resetTimerfd
          ::timerfd_settime
        ]
      }
    ]
  ]

}



/**处理定时事件的发生 */

timer expiration->timerfdchannel readable
timerfdchannel.handleEvent
readcb
{//TimerQueue::handleRead
  Timestamp now;
  readTimerfd(timerfd,now)
  [
    ::read(timerfd,uint64_t)
  ]
  vector<{Timestamp,Timer*}> expired = getExpired(now)
  [//TimerQueue::getExpired(Timestamp)
    根据当前时间,把已经到期的timer都取出来(erase from timers)
  ]
  for e : expired
  {
    /**执行用户指定的cb */
    e->second->run();

  }
  /** */
  reset(expired)
  [//TimerQueue::reset(vector<Entry>)
    把interval类型的timer重新放回到timers中
    根据timers中的内容,设定timerfd的到期时间
  ]
}