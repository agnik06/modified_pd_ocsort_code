
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>
#include <vector>
#include <stdexcept>

#ifdef __android__
#include <android/log.h>
#endif

#ifndef LOG_TAG
#define LOG_TAG "VIZ_PET_DETECTOR_MULTIF"
#endif

namespace vizpet {
/*!
 * \brief exception class that will be thrown by
 *  default logger if DMLC_LOG_FATAL_THROW == 1
 */
struct Error : public std::runtime_error {
  /*!
   * \brief constructor
   * \param s the error message
   */
  explicit Error(const std::string &s) : std::runtime_error(s) {}
};
}  // namespace vizpet

// use a light version of glog
#include <assert.h>
#include <iostream>
#include <sstream>
#include <ctime>

//TODO: Replace Old version of logger
#ifdef __android__
#include <android/log.h>
#ifndef APPNAME
#define APPNAME "ImageSearchJNI_PET_MULTIF"
#endif
#ifndef LOGV
#define LOGV(fmt, ...)                                              \
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "%s: %d: " fmt, \
                      __FILENAME__, __LINE__, ##__VA_ARGS__);
#endif
#else
// #elif __terminal_logging__
#define LOGV(...)                            \
  printf("%s-%d: ", __FILENAME__, __LINE__); \
  printf(__VA_ARGS__);                       \
  printf("\n");
// #else
// #define LOGV(...) ;
#endif


#if defined(_MSC_VER)
#pragma warning(disable : 4722)
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__,'/') + 1 : __FILE__)

namespace vizpet {

class LogCheckError {
 public:
  LogCheckError() : str(nullptr) {}
  explicit LogCheckError(const std::string& str_) : str(new std::string(str_)) {}
  ~LogCheckError() { if (str != nullptr) delete str; }
  operator bool() {return str != nullptr; }
  std::string* str;
};

#define DEFINE_CHECK_FUNC(name, op)                               \
  template <typename X, typename Y>                               \
  inline LogCheckError LogCheck##name(const X& x, const Y& y) {   \
    if (x op y) return LogCheckError();                           \
    std::ostringstream os;                                        \
    os << " (" << x << " vs. " << y << ") ";  /* CHECK_XX(x, y) requires x and y can be serialized to string. Use CHECK(x OP y) otherwise. NOLINT(*) */ \
    return LogCheckError(os.str());                               \
  }                                                               \
  inline LogCheckError LogCheck##name(int x, int y) {             \
    return LogCheck##name<int, int>(x, y);                        \
  }

#define CHECK_BINARY_OP(name, op, x, y)                                 \
  if (vizpet::LogCheckError _check_err = vizpet::LogCheck##name(x, y))    \
    vizpet::LogMessageFatal(__FILE__, __LINE__).stream()                 \
      << "Check failed: " << #x " " #op " " #y << *(_check_err.str)

DEFINE_CHECK_FUNC(_LT, <)
DEFINE_CHECK_FUNC(_GT, >)
DEFINE_CHECK_FUNC(_LE, <=)
DEFINE_CHECK_FUNC(_GE, >=)
DEFINE_CHECK_FUNC(_EQ, ==)
DEFINE_CHECK_FUNC(_NE, !=)

// Always-on checking
#define CHECK(x)                                            \
  if (!(x))                                                 \
    vizpet::LogMessageFatal(__FILE__, __LINE__).stream()     \
      << "Check failed: " #x << ' '
#define CHECK_LT(x, y) CHECK_BINARY_OP(_LT, <, x, y)
#define CHECK_GT(x, y) CHECK_BINARY_OP(_GT, >, x, y)
#define CHECK_LE(x, y) CHECK_BINARY_OP(_LE, <=, x, y)
#define CHECK_GE(x, y) CHECK_BINARY_OP(_GE, >=, x, y)
#define CHECK_EQ(x, y) CHECK_BINARY_OP(_EQ, ==, x, y)
#define CHECK_NE(x, y) CHECK_BINARY_OP(_NE, !=, x, y)
#define CHECK_NOTNULL(x) \
  ((x) == NULL ? vizpet::LogMessageFatal(__FILE__, __LINE__).stream() << "Check  notnull: "  #x << ' ', (x) : (x)) // NOLINT(*)
// Debug-only checking.
#ifdef NDEBUG
#define DCHECK(x) \
  while (false) CHECK(x)
#define DCHECK_LT(x, y) \
  while (false) CHECK((x) < (y))
#define DCHECK_GT(x, y) \
  while (false) CHECK((x) > (y))
#define DCHECK_LE(x, y) \
  while (false) CHECK((x) <= (y))
#define DCHECK_GE(x, y) \
  while (false) CHECK((x) >= (y))
#define DCHECK_EQ(x, y) \
  while (false) CHECK((x) == (y))
#define DCHECK_NE(x, y) \
  while (false) CHECK((x) != (y))
#else
#define DCHECK(x) CHECK(x)
#define DCHECK_LT(x, y) CHECK((x) < (y))
#define DCHECK_GT(x, y) CHECK((x) > (y))
#define DCHECK_LE(x, y) CHECK((x) <= (y))
#define DCHECK_GE(x, y) CHECK((x) >= (y))
#define DCHECK_EQ(x, y) CHECK((x) == (y))
#define DCHECK_NE(x, y) CHECK((x) != (y))
#endif  // NDEBUG

#ifndef __android__
#define ANDROID_LOG_VERBOSE 0
#define ANDROID_LOG_INFO 0
#define ANDROID_LOG_ERROR 0
#define ANDROID_LOG_WARN 0
#endif
 
#define LOG_INFO vizpet::LogMessage(ANDROID_LOG_INFO, __FILENAME__, __LINE__)
#define LOG_ERROR vizpet::LogMessage(ANDROID_LOG_ERROR, __FILENAME__, __LINE__)
#define LOG_WARNING vizpet::LogMessage(ANDROID_LOG_WARN, __FILENAME__, __LINE__)
#define LOG_FATAL vizpet::LogMessageFatal(__FILENAME__, __LINE__)
#define LOG_QFATAL LOG_FATAL

#ifdef DEBUG_LOG
  #define LOG_INFO_DEBUG vizpet::LogMessage(ANDROID_LOG_INFO, __FILENAME__, __LINE__)
#else
  #define LOG_INFO_DEBUG vizpet::LogMessageDummy(ANDROID_LOG_INFO, __FILENAME__, __LINE__)
#endif

// Poor man version of VLOG
#define VLOG(x) LOG_INFO.stream()<<(x);

#define LOG(severity) LOG_##severity.stream()
#define LG_DEBUG LOG_INFO_DEBUG.stream()
#define LG LOG_INFO.stream()
#define LOG_IF(severity, condition) \
  !(condition) ? (void)0 : vizpet::LogMessageVoidify() & LOG(severity)

#ifdef NDEBUG
#define LOG_DFATAL LOG_ERROR
#define DFATAL ERROR
#define DLOG(severity) true ? (void)0 : vizpet::LogMessageVoidify() & LOG(severity)
#define DLOG_IF(severity, condition) \
  (true || !(condition)) ? (void)0 : vizpet::LogMessageVoidify() & LOG(severity)
#else
#define LOG_DFATAL LOG_FATAL
#define DFATAL FATAL
#define DLOG(severity) LOG(severity)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#endif

// Poor man version of LOG_EVERY_N
#define LOG_EVERY_N(severity, n) LOG(severity)

class DateLogger {
 public:
  DateLogger() {
#if defined(_MSC_VER)
    _tzset();
#endif
  }
  const char* HumanDate() {
#if defined(_MSC_VER)
    _strtime_s(buffer_, sizeof(buffer_));
#else
    time_t time_value = time(NULL);
    struct tm *pnow;
#if !defined(_WIN32)
    struct tm now;
    pnow = localtime_r(&time_value, &now);
#else
    pnow = localtime(&time_value);  // NOLINT(*)
#endif
    snprintf(buffer_, sizeof(buffer_), "%02d:%02d:%02d",
             pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
#endif
    return buffer_;
  }

 private:
  char buffer_[9];
};

class LogMessage {
 public:
  void init_message(const char* file, int line){
    #ifdef DEBUG_LOG
        log_stream_ << "[" << pretty_date_.HumanDate() << "] " << file << ":"
                    << line << ": ";
    #endif
  }
  LogMessage():
    priority(ANDROID_LOG_VERBOSE){
      init_message("unknown", 0);
  }

  LogMessage(int prio, const char* file, int line):
    priority(prio){
      init_message(file, line);
  }
  LogMessage(const char* file, int line):
    priority(ANDROID_LOG_VERBOSE){
      init_message(file, line);
  }

  ~LogMessage() {
    log_stream_ << '\n';
#ifdef __android__
    __android_log_print(priority, LOG_TAG, "%s", log_stream_.str().c_str());
#endif
  }
  std::ostream& stream() { return log_stream_; }

 protected:
#ifndef __android__
  std::ostream& log_stream_ = std::cout;
#else
  std::ostringstream log_stream_;
#endif

 private:
  int priority;
  DateLogger pretty_date_;
  LogMessage(const LogMessage&);
  void operator=(const LogMessage&);
};

class NullBuf: public std::streambuf{
public:
  int overflow(int c){return c;}
};

class LogMessageDummy{
public:
  LogMessageDummy(int prio, const char* file, int line):
    nstream(&nbuf){
  }
  LogMessageDummy(const char* file, int line):
    nstream(&nbuf){
  }
  ~LogMessageDummy() {}
  std::ostream& stream() { return nstream; }

private:
  std::ostream nstream;
  NullBuf nbuf;
};

class LogMessageFatal {
 public:
  LogMessageFatal(const char* file, int line) {
    log_stream_ << "[" << pretty_date_.HumanDate() << "] " << file << ":"
                << line << ": ";
  }
#if defined(_MSC_VER) && _MSC_VER < 1900
  ~LogMessageFatal() {
#else
  ~LogMessageFatal() noexcept(false) {
#endif
    LOG(ERROR) << log_stream_.str();
    throw Error(log_stream_.str());
  }
  std::ostringstream &stream() { return log_stream_; }

 private:
  std::ostringstream log_stream_;
  DateLogger pretty_date_;
  LogMessageFatal(const LogMessageFatal&);
  void operator=(const LogMessageFatal&);
};

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify {
 public:
  LogMessageVoidify() {}
  // This has to be an operator with a precedence lower than << but
  // higher than "?:". See its usage.
  void operator&(std::ostream&) {}
};

}  // namespace vizpet

#endif  // viz_LOGGER_H_