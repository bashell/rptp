#include <signal.h>
#include "TcpServer.h"

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


class IgnoreSigpipe {
 public:
  IgnoreSigpipe() {
    if(::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
      ERR_EXIT("signal");
  }
};

IgnoreSigpipe initObj;  // 全局对象, 系统初始化时一定会处理SIGPIPE



TcpServer::TcpServer(const InetAddress &addr) {
  int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
    ERR_EXIT("socket");
  sock_.reset(new Socket(sockfd));  // 释放sock_原来指向的对象, 并指向new出来的新对象
  sock_->setReusePort();            // 设置端口复用
  sock_->bindAddress(addr);         // bind
  sock_->listen();                  // listen
}


void TcpServer::start() {
  poller_.reset(new PollPoller(sock_->fd()));
  poller_->setConnectionCallback(onConnect_);
  poller_->setMessageCallback(onMessage_);
  poller_->setCloseCallback(onClose_);

  while(1) {
    poller_->poll();
    poller_->handleAccept();
    poller_->handleData();
  }
}


void TcpServer::setConnection(const TcpCallback &cb) {
  onConnect_ = cb;
}


void TcpServer::setMessage(const TcpCallback &cb) {
  onMessage_ = cb;
}


void TcpServer::setClose(const TcpCallback &cb) {
  onClose_ = cb;
}


