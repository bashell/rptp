#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <functional>

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


class Thread {
 public:
  using ThreadFunc = std::function<void()>;  // 可调用类型形如: void m()

  Thread();
  explicit Thread(const ThreadFunc&);  // 抑制构造函数隐式转换
  ~Thread();

  void setCallback(const ThreadFunc&);  // 设置回调函数
  void start();  // 线程开启
  void join();   // 阻塞至线程终止

 private:
  Thread(const Thread&) = delete;
  Thread &operator=(const Thread&) = delete;

  static void *threadFunc(void*);

 private:
  pthread_t tid_;
  bool isStarted_;
  ThreadFunc callback_;
};



inline Thread::Thread() : tid_(0), isStarted_(false) {

}


inline Thread::Thread(const ThreadFunc &cb)
    : tid_(0), isStarted_(false), callback_(cb) {
    
}


inline Thread::~Thread() {
  if(isStarted_)
    pthread_detach(tid_);  // 分离线程
}


inline void Thread::setCallback(const ThreadFunc &cb) {
  callback_ = cb;
}


inline void Thread::start() {
  isStarted_ = true;
  if(pthread_create(&tid_, NULL, threadFunc, this) != 0)  // 将this指针(调用对象的地址)传给threadFunc
    ERR_EXIT("pthread_create");
}


inline void Thread::join() {
  if(pthread_join(tid_, NULL) != 0)
    ERR_EXIT("pthread_join");
  isStarted_ = false;
}


inline void* Thread::threadFunc(void *arg) {
  Thread *p = static_cast<Thread *>(arg);
  p->callback_();  // 调用线程逻辑
  return NULL;
}


#endif  /* _THREAD_H_ */
