#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <memory>
#include <vector>
#include <queue>
#include <functional>
#include <cstddef>
#include "MutexLock.h"
#include "Condition.h"
#include "Thread.h"

#define DEFAULT_QUEUE_SIZE 10
#define DEFAULT_POOL_SIZE  5


class ThreadPool {
 public:
  using Task = std::function<void()>;  // 任务类型: void()

  ThreadPool();
  ThreadPool(size_t queueSize, size_t poolSize);
  ~ThreadPool();

  void start();  // 开启线程池
  void stop();   // 停止线程池
  void addTask(const Task&);  // 添加任务

 private:
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool &operator=(const ThreadPool&) = delete;

  Task getTask();
  void runInThread();  // 线程池内线程的回调函数
 
 private:
  mutable MutexLock mutex_;  // 使得mutex_在const成员函数中也可以改变状态
  Condition empty_;
  Condition full_;
  size_t queueSize_;        // 同步队列大小
  std::queue<Task> queue_;  // 同步队列
  size_t poolSize_;         // 线程池大小
  std::vector<std::unique_ptr<Thread>> threads_;  // 线程池
  bool isStarted_;          // 是否开启标志
};


inline ThreadPool::ThreadPool()
    : empty_(mutex_),
      full_(mutex_),
      queueSize_(DEFAULT_QUEUE_SIZE),
      poolSize_(DEFAULT_POOL_SIZE),
      isStarted_(false)
{
}


inline ThreadPool::ThreadPool(size_t queueSize, size_t poolSize)
    : empty_(mutex_),
      full_(mutex_),
      queueSize_(queueSize),
      poolSize_(poolSize),
      isStarted_(false)  
{
}


inline ThreadPool::~ThreadPool() {
  if(isStarted_)  // 析构前确保线程池终止运行
    stop();
}


inline void ThreadPool::start() {
  if(isStarted_) return;
  isStarted_ = true;

  // 初始化线程池
  for(size_t i = 0; i != poolSize_; ++i)
    /*
     * 调用Thread类的构造函数 Thread(const ThreadFunc&) 来创建新线程.
     * 将ThreadPool的成员函数runInThread和调用此start()函数的对象的地址进行绑定, 达到延迟调用的目的
     * 当进行调用时, 可以通过对象的地址找到该对象的runInThread方法进行调用.
     */
    threads_.push_back(std::unique_ptr<Thread>(new Thread(std::bind(&ThreadPool::runInThread, this))));

  // 开启各个线程
  for(size_t i = 0; i != poolSize_; ++i)
    threads_[i]->start();
}


inline void ThreadPool::stop() {
  if(isStarted_ == false) return;

  {
    MutexLockGuard lock(mutex_);
    isStarted_ = false;
    full_.signal_all();  // 唤醒线程
  }

  // 等待线程终止
  for(size_t i = 0; i != poolSize_; ++i)
    threads_[i]->join();

  // 清空同步队列
  while(!queue_.empty())
    queue_.pop();

  // 清空线程池
  threads_.clear();
}


// producer
inline void ThreadPool::addTask(const Task &task) {
  MutexLockGuard lock(mutex_);
  while(queue_.size() >= queueSize_)
    empty_.wait();
  queue_.push(task);
  full_.signal_one();  // 通知consumer
}


// consumer
inline ThreadPool::Task ThreadPool::getTask() {
  MutexLockGuard lock(mutex_);
  while(queue_.empty() && isStarted_)
    full_.wait();
  /*
   * 这里的wait可以被stop()中的signal_all()激活,
   * 此时isStarted_ == false, 退出循环后, queue_仍为空
   */
  Task task;
  // 若队列queue_为空, 则返回一个空函数
  // 若队列queue_不为空, 则获得任务
  if(!queue_.empty()) {
    task = queue_.front();
    queue_.pop();
    empty_.signal_one();  // 通知producer
  }
  return task;
}


inline void ThreadPool::runInThread() {
  while(isStarted_ == true) {
    Task task(getTask());
    if(task) {
      task();  // 执行任务
    }
  }
}


#endif  /* _THREAD_POOL_H_ */
