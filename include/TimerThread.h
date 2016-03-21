#ifndef _TIMERTHREAD_H_
#define _TIMERTHREAD_H_

#include <functional>
#include "Thread.h"
#include "Timer.h"

class TimerThread {
 public:
  using TimerCallback = std::function<void()>;
  TimerThread();
  void setTimer(int value, int interval);
  void setTimerCallback(const TimerCallback &cb);
  void startTimerThread();
  void cancelTimerThread();

 private:
  TimerThread(const TimerThread&) = delete;
  TimerThread &operator=(const TimerThread&) = delete;

 private:
  Timer timer_;
  Thread thread_;
};


#endif  /* _TIMERTHREAD_H_ */
