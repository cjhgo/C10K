/**处理连接的建立 */


/** server socket的注册 */

TcpServer tcpserver(&loop, listenAddr)
{//TcpServer::Tcpserver(EventLoop*, InetAddress&)构造函数
  acceptor = new Acceptor(loop,listenAddr)
  [//Acceptor::Acceptor(EventLoop*, InetAddress&)
    acceptsocket = sockets::createNonblockingSocket
    acceptchanel = Chanel(loop,acceptsocket.fd)
    acceptsocket.setreuseaddr
    acceptsocket.bindaddress(listenAddr)
    accepetchannel.setreadcb(&Acceptor::handleRead)
  ]
  acceptor.setNewConnCb(TcpServer::newConnn)
}

tcpserver.start
{//TcpServer::start
  loop->runInLoop(Acceptor::listen,)
  [//Acceptor::listen
    acceptsocket.listen()
    acceptchannel.enableReading()
  ]

}


 newconnection  -> server socket readable 
 socketchannel.handleEvent
 readcb
 {//Acceptor::handleRead
  InetAddr peerAddr
  int connfd = acceptsocket.accept(peerAddr)
  if( connfd > 0)
  {
    if(nccb)
    {
      nccb(connfd, peerAddr)//acceptor.setNewConnCb(TcpServer::newConnn)
      [//TcpServer::newConnection(int, InetAddress)
        
      ]
    }
  }

 }