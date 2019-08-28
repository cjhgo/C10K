#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>

namespace libnet
{
/**
 * \def LOG_FATAL
 *   Used for fatal and probably irrecoverable conditions
 * \def LOG_ERROR
 *   Used for errors which are recoverable within the scope of the function
 * \def LOG_WARNING
 *   Logs interesting conditions which are probably not fatal
 * \def LOG_EMPH
 *   Outputs as LOG_INFO, but in LOG_WARNING colors. Useful for
 *   outputting information you want to emphasize.
 * \def LOG_INFO
 *   Used for providing general useful information
 * \def LOG_DEBUG
 *   Debugging purposes only
 * \def LOG_EVERYTHING
 *   Log everything 
 *
 * \def OUTPUTLEVEL
 *  The minimum level to logger at
 * \def LOG_NONE
 *  OUTPUTLEVEL to LOG_NONE to disable logging
 */


enum LogLevel
{
  NONE=7,
  FATAL=6,
  ERROR=5,
  WARNING=4,
  EMPH=3,
  INFO=2,
  DEBUG=1,
  EVERYTHING=0
};

class Logger
{
 public:

  // Logger();
  static Logger& instance()
  {
    static Logger logger;
    return logger;
  }
  void setLogLevel(LogLevel level)
  {
    min_log_level = level;
  }

  Logger& setcurLogLevel(LogLevel level)
  {
    cur_log_level = level;
    return *this;
  }

  
  template<typename T>
  
  Logger& operator<<(const T& val) 
  {
    if( cur_log_level >= min_log_level)
    {
      std::cout<<val;
    }
    return *this;
  }
 private:
  LogLevel cur_log_level;
  LogLevel min_log_level;
};



#define LOG_NONE Logger::instance().setcurLogLevel(NONE)<<__FILE__<<__LINE__<<": \n"
#define LOG_FATAL Logger::instance().setcurLogLevel(FATAL)<<__FILE__<<__LINE__<<": \n"
#define LOG_ERROR Logger::instance().setcurLogLevel(ERROR)<<__FILE__<<__LINE__<<": \n"
#define LOG_WARNING Logger::instance().setcurLogLevel(WARNING)<<__FILE__<<__LINE__<<": \n"
#define LOG_EMPH Logger::instance().setcurLogLevel(EMPH)<<__FILE__<<__LINE__<<": \n"
#define LOG_INFO Logger::instance().setcurLogLevel(INFO)<<__FILE__<<__LINE__<<": \n"
#define LOG_DEBUG Logger::instance().setcurLogLevel(DEBUG)<<__FILE__<<__LINE__<<": \n"
#define LOG_EVERYTHING Logger::instance().setcurLogLevel(EVERYTHING)<<__FILE__<<__LINE__<<": \n"
}


#endif