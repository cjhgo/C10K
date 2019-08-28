
/**
 * eventloop的执行逻辑
 */



/** 注册event */

channel.(enableReading/enableWriing/disableWrting/disableAll)
{
  set event
  update()
  [//Channel::update
    loop->updateChannel(this);
    [//EventLoop::updateChannel(Channel*)
      poller->updateChannel(channel)
      [//Poller::updateChannel(Channel*)
        pollfds.push_back(chanel->fd)
        channel->set_index()
        channels[fd]=channel
      ]
    ]
  ]
}

/**执行io loop */
loop.loop()
{//EventLoop::loop
  while (!quit)
  {
    /*把poll调用返回的活跃channel填充到参数中*/
    prt = poller->poll(activechannels)
    [//Poller::poll
      int nume = ::poll(fds, nfds,timeout)
      fillActiveChannel(nume, activechannels)
      [//Poller::fillActiveChannels
        for pfd : pfds
        {
          if( pfd.revent)
          {
            channel = channels->find(pfd)
            channel.setrevent(pfd.revent)
            activechannels.push_back(channel)
          }

        }
      ]
    ]
    /*处理活跃的channel*/
    for channel : activechannels
    {
      channel->handleEvent(prt)
      [//Channel::handleEvent
        if inevent: readCb()
        if outevent: writecb()
        if closeevent: closecb()
        if errorevent: errorcb()
      ]
    }
  }
  /*处理阻塞期间queue进来的functor */
  doPendingFunctors();
  [//EventLoop::doPendingFunctors
    vector<Functor> todo;
    {
      lock
      functors.swap(todo)
    }
    for e : todo
    {
      e()
    }
  ]
}