#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "InetAddress.h"


class Socket {
 public:
  explicit Socket(int);  // 抑制构造函数隐式转换
  ~Socket();

  int fd() const;
  void bindAddress(const InetAddress &);
  void listen();
  int accept();
  void shutdownWrite();  // 关闭写端
  void setReusePort();   // 端口复用

 private:
  Socket(const Socket&) = delete;
  Socket &operator=(const Socket&) = delete;

 private:
  const int sockfd_;
};

#endif  /* _SOCKET_H_ */
