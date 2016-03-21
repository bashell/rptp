#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/timerfd.h>
#include <functional>

class Timer {
 public:
  using TimeCallback = std::function<void()>;
  Timer();
  ~Timer();
  void setTime(int value, int interval);
  void setTimeCallback(const TimeCallback&);
  void runTimer();
  void cancelTimer();
  
 private:
  Timer(const Timer&) = delete;
  Timer &operator=(const Timer&) = delete;

 private:
  int timerfd_;
  struct itimerspec howlong_;
  TimeCallback timeCallback_;  // 用户逻辑
  bool isStarted_;  // 开启标志
};


#endif  /* _TIMER_H_ */
