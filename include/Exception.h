#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <stdlib.h>
#include <execinfo.h>

// 编译时使用-rdynamic选项, 确保stackTrace带有名字信息


class Exception : public std::exception {
 public:
  explicit Exception(const char*);
  explicit Exception(const std::string&);
  virtual ~Exception() throw();  // 此函数不抛出异常
  virtual const char *what() const throw();
  const char *stackTrace() const throw();

 private:
  void fillStackTrace();  // 填充栈痕迹

 private:
  std::string message_;  // 异常名字
  std::string stack_;    // 栈痕迹
};


inline Exception::Exception(const char *s) : message_(s) {
  fillStackTrace();
}


inline Exception::Exception(const std::string &s) : message_(s) {
  fillStackTrace();
}


inline Exception::~Exception() throw() {

}


inline const char* Exception::what() const throw() {
  return message_.c_str();
}


inline const char* Exception::stackTrace() const throw() {
  return stack_.c_str();
}


inline void Exception::fillStackTrace() {
  const int len = 200;
  void *buffer[len];
  // 获取栈的调用信息
  int nptrs = ::backtrace(buffer, len);
  // 翻译成可读的字符串
  char **strings = ::backtrace_symbols(buffer, nptrs);
  if(strings) {
    for(int i = 0; i < nptrs; ++i) {
      // TODO demangle funcion name with abi::__cxa_demangle
      stack_.append(strings[i]);
      stack_.push_back('\n');
    }
    free(strings);
  }
}



#endif  /* _EXCEPTION_H_ */
