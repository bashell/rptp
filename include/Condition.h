#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "MutexLock.h"


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
  if(pthread_cond_init(&cond_, NULL) != 0)
    std::cerr << "cond init" << std::endl;
}


inline Condition::~Condition() {
  if(pthread_cond_destroy(&cond_) != 0)
    std::cerr << "cond destroy" << std::endl;
}


inline void Condition::wait() {
  if(pthread_cond_wait(&cond_, mutex_.getMutexPtr()) != 0)
    std::cerr << "cond wait" << std::endl;
}

inline void Condition::signal_one() {
  if(pthread_cond_signal(&cond_) != 0)
    std::cerr << "cond signal" << std::endl;
}

inline void Condition::signal_all() {
  if(pthread_cond_broadcast(&cond_) != 0)
    std::cerr << "cond broadcast" << std::endl;
}


#endif  /* _CONDITION_H_ */
