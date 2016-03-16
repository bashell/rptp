#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "MutexLock.h"


//class MutexLock;

class Condition {
 public:
  Condition(MutexLock&);
  ~Condition();

  void wait();
  void signal_one();
  void signal_all();

 private:
  Condition(const Condition&) = delete;
  Condition &operator=(const Condition&) = delete;

 private:
  pthread_cond_t cond_;
  MutexLock &mutex_;
};



inline Condition::Condition(MutexLock &mut) : mutex_(mut) {
  pthread_cond_init(&cond_, NULL);    
}


inline Condition::~Condition() {
  pthread_cond_destroy(&cond_);
}


inline void Condition::wait() {
  assert(mutex_.isLocked());  // 锁住才能wait
  pthread_cond_wait(&cond_, mutex_.getMutexPtr());
}

inline void Condition::signal_one() {
  pthread_cond_signal(&cond_);
}

inline void Condition::signal_all() {
  pthread_cond_broadcast(&cond_);
}


#endif  /* _CONDITION_H_ */
