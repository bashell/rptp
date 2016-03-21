#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include "Timer.h"

#define ERR_EXIT(m) \
    do { \
      perror(m); \
      exit(EXIT_FAILURE); \
    } while(0)



Timer::Timer() : isStarted_(false) {
  timerfd_ = ::timerfd_create(CLOCK_REALTIME, 0);
  if(timerfd_ == -1)
    ERR_EXIT("timerfd_create");
  ::memset(&howlong_, 0, sizeof(howlong_));
}


Timer::~Timer() {
  ::close(timerfd_);
}


void Timer::setTime(int value, int interval) {
  howlong_.it_value.tv_sec = value;        // 初始超时时间
  howlong_.it_interval.tv_sec = interval;  // 之后的超时时间
}


void Timer::setTimeCallback(const TimeCallback &cb) {
  timeCallback_ = cb;
}


void Timer::runTimer() {
  struct pollfd event[1];
  event[0].fd = timerfd_;
  event[0].events = POLLIN;
  char buf[1024];
  int nready;

  if(timerfd_settime(timerfd_, 0, &howlong_, NULL) == -1)
    ERR_EXIT("timerfd_settime");
  isStarted_ = true;
  while(isStarted_) {
    nready = poll(event, 1, 10000);
    if(nready == -1)
      ERR_EXIT("poll");
    else if(nready == 0)
      printf("timeout!\n");
    else {
      if(read(timerfd_, buf, sizeof(buf)) == -1)
        ERR_EXIT("read");
      timeCallback_();  // 调用用户逻辑
    }
  }
}


void Timer::cancelTimer() {
  memset(&howlong_, 0, sizeof(howlong_));
  if(timerfd_settime(timerfd_, 0, &howlong_, NULL) == -1)
    ERR_EXIT("timerfd_settime");
  isStarted_ = false;
}
