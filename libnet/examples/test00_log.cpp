#include "libnet/libnet.hpp"

using namespace libnet;


int main(int argc, char const *argv[])
{
  // Logger logger =  Logger::instance();  
  
  Logger::instance().setLogLevel(EVERYTHING);

  LOG_FATAL<<"wow this is a log"<<33<<"\n";
  return 0;
}

