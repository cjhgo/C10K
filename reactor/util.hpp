#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <cstdio>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>


int handleErr(int err, const char msg[],bool exitnow=false)
{
  if( err == -1)
  {
    std::stringstream ss;
    ss<<"the errno is "<<errno<<"\t";
    ss<<std::string(msg);    
    perror(ss.str().c_str());
    if(exitnow)exit(EXIT_FAILURE);
  }
  return err;
}

#endif