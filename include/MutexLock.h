#ifndef _MUTEX_LOCK_H_
#define _MUTEX_LOCK_H_

#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


class MutexLock {
  friend class MutexLockGuard;
 public:
  MutexLock();
  ~MutexLock();
  bool isLocked() const;
  pthread_mutex_t* getMutexPtr();
 
 private:
  MutexLock(const MutexLock&) = delete;
  MutexLock &operator=(const MutexLock&) = delete;
  
  // 防止用户手工调用
  void lock();
  void unlock();
 
 private:
  pthread_mutex_t mutex_;
  bool isLocked_;
};


// RAII
class MutexLockGuard {
 public:
  MutexLockGuard(MutexLock &mutex);  // 资源获取即初始化
  ~MutexLockGuard();                 // 析构时释放资源

 private:
  MutexLockGuard(const MutexLockGuard&) = delete;
  MutexLockGuard &operator=(const MutexLockGuard&) = delete;
  
 private:
  MutexLock &mutex_;
};



/************* MutexLock *************/
inline MutexLock::MutexLock() : isLocked_(false) {
  if(pthread_mutex_init(&mutex_, NULL) != 0)
    ERR_EXIT("pthread_mutex_init");
}


inline MutexLock::~MutexLock() {
  assert(isLocked_ == false);
  pthread_mutex_destroy(&mutex_);
}


inline bool MutexLock::isLocked() const {
  return isLocked_;
}


inline pthread_mutex_t* MutexLock::getMutexPtr() {
  return &mutex_;
}


inline void MutexLock::lock() {
  pthread_mutex_lock(&mutex_);
  isLocked_ = true;
}


inline void MutexLock::unlock() {
  isLocked_ = false;
  pthread_mutex_unlock(&mutex_);
}



/********** MutexLockGuard **********/
inline MutexLockGuard::MutexLockGuard(MutexLock &mut)
    : mutex_(mut) {
  mutex_.lock();    
}

inline MutexLockGuard::~MutexLockGuard() {
  mutex_.unlock();
}


// 帮助在编译期发现错误
#define MutexLockGuard(m) "ERROR"


#endif  /* _MUTEX_LOCK_H_ */
