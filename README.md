# rptp

### 开发环境
- Ubuntu 14.04LTS
- g++ 4.8.4

### 编程模式
- 单线程Reactor模式：Non-Blocking I/O + I/O multiplexing（这里选择poll）
- Reactor + ThreadPool ：把计算任务交给线程池， I/O线程只负责I/O任务


### 概要说明
- rptp借鉴了大量的muduo代码，并对其进行简化
- rptp体现了现代C++的两大风格
	- 使用function/bind实现回调机制。用户只需要向其注册回调函数即可。
	- 采用智能指针进行资源管理。例如TcpConnection使用了shared\_ptr管理其生命周期，其它类借助了unique\_ptr的销毁功能。
- rptp的不足之处
	- 对错误的处理比较粗糙
	- 没有使用比较规范的日志 
- rptp与muduo的主要差距：
	- rptp没有实现复用同一个Poller的功能，这正是muduo中的EventLoop（里面封装了一个通用的Poll、Epoll模型）
	- rptp采用的是阻塞I/O，不能成为严格意义上的Reactor模式。如果网络拥塞，I/O会阻塞在read函数上，这样会降低整个系统的并发能力
	- 没有实现缓冲区

### TO DO
- 增加非阻塞I/O
- 实现高性能的缓冲区

### 源码分析
1. rptp中除了InetAddress和Exception之外，其余类均通过=delete禁止拷贝和赋值
2. Exception相比标准库的exception增加了打印栈痕迹的功能
3. ThreadPool系列（MutexLock、Condition、Thread、ThreadPool）
	- 大量采用RAII技术，避免资源泄露
	- 对于Thread和ThreadPool，采用function作为泛型技术，用户只需注册回调函数
4. Timer/TimerThread
	- 内部采用timerfd系列的定时器，不使用信号，而是使用fd可读作为定时器的触发事件，使得Timer可以加入到I/O复用模型
	- TimerThread为Timer的线程封装
5. TcpServer系列（Rio、Socket、InetAddress、TcpConnection、TcpServer）
	- Rio封装了网络编程中的三大函数readn、readline、writen
	- Socket则封装了基本的socket操作，但是不包含读写
	- InetAddress包装了sockaddr_in结构体，使之更易使用
	- TcpConnection包装了Tcp连接的各种操作，主要是数据的收发以及获取Tcp连接的信息
	- TcpConnection采用shared\_ptr管理其生存期，还继承了enable\_shared\_from\_this，用于在类的内部获取自己的智能指针
	- PollPoller封装了Poll模型，内部使用map<int, TcpConnectionPtr>实现从文件描述符到Tcp连接的映射
	- TcpServer是对以上组件的组合
6. 用户注册事件与回调流程：
	- 注册事件顺序：TcpServer ---> PollPoller ---> TcpConnection
	- 回调流程：回调函数由PollPoller触发，通过map寻找到Tcp连接，然后调用里面的回调函数
7. TcpServer实质是一个I/O复用模型，ThreadPool代表多线程，用户在使用时，可以只选择其一。  如果计算任务负担较重，可以将计算任务与Tcp回发封装成函数，交给线程池去计算。此时，运行TcpServer的线程是一个I/O线程，ThreadPool里面的线程专注于CPU密集型计算

### 使用说明
- 计算任务轻时：只使用TcpServer
- 计算任务重时：组合使用TcpServer和ThreadPool。Tcp回发，计算任务分发给线程池

### 安装方法
```bash
# git clone https://github.com/bashell/rptp.git
# cd rptp
# make
# sudo cp -r rptp /usr/include/
# sudo cp librptp.a /usr/lib/
```
注：编译时需要加上 -std=c++11 -lrptp
