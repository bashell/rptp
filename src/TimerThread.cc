#include "TimerThread.h"

TimerThread::TimerThread() : thread_(std::bind(&Timer::runTimer, &timer_)) {

}

void TimerThread::setTimer(int value, int interval) {
  timer_.setTime(value, interval);
}

void TimerThread::setTimerCallback(const TimerCallback &cb) {
  timer_.setTimeCallback(cb);
}

void TimerThread::startTimerThread() {
  thread_.start();
}

void TimerThread::cancelTimerThread() {
  timer_.cancelTimer();
  thread_.join();
}
