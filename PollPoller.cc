#include <iostream>
#include <utility>  // pair
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "PollPoller.h"

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


PollPoller::PollPoller(int listenfd) : listenfd_(listenfd) {
  for(int i = 0; i != 2048; ++i)
    event_[i].fd = -1;
  event_[0].fd = listenfd;
  event_[0].events = POLLIN;
  maxi_ = nready_ = 0;
}


PollPoller::~PollPoller() {

}


void PollPoller::poll() {
  int ret;
  do {
    ret = ::poll(event_, maxi_ + 1, 10000);
  } while(ret == -1 && errno == EINTR);
  if(ret == -1)  // 其它ERROR
    ERR_EXIT("poll");
  nready_ = ret;
}


void PollPoller::handleAccept() {
  if(event_[0].revents & POLLIN) {
    int peerfd;
    if((peerfd = ::accept(listenfd_, NULL, NULL)) == -1)
      ERR_EXIT("accept");
    handleConnectionEvent(peerfd);
  }
}


void PollPoller::handleData() {
  // 依次处理每个客户
  for(int i = 1; i <= maxi_; ++i) {
    int peerfd = event_[i].fd;
    if(peerfd == -1)
      continue;
    if(event_[i].revents & POLLIN) {
      char buf[100];
      int nready = ::recv(peerfd, buf, sizeof(buf), MSG_PEEK);  // 查看但不取走数据
      if(nready == -1)
        ERR_EXIT("recv");
      else if(nready == 0)  // 客户fd关闭
        handleCloseEvent(i);
      else  // 处理消息事件
        handleMessageEvent(peerfd);
    }
  }
}


void PollPoller::handleConnectionEvent(int peerfd) {
  // 1. 添加fd至event_
  int i;
  for(i = 1; i < 2048; ++i) {
    if(event_[i].fd == -1) {  // 找到第一个未用fd
      event_[i].fd = peerfd;
      event_[i].events = POLLIN;
      if(i > maxi_)
        maxi_ = i;
      break;
    }
  }
  if(i == 2048) {
    ::fprintf(stderr, "too many clients\n");
    ::exit(EXIT_FAILURE);
  }

  TcpConnectionPtr conn(new TcpConnection(peerfd,
                        InetAddress::getLocalAddress(peerfd),
                        InetAddress::getPeerAddress(peerfd)));
  conn->setConnectionCallback(onConnectionCallback_);
  conn->setMessageCallback(onMessageCallback_);
  conn->setCloseCallback(onCloseCallback_);

  // 2. map中新增一条
  std::pair<TcpIterator, bool> ret = lists_.insert(std::make_pair(peerfd, conn));
  
  // 3. 调用函数
  assert(ret.second == true);
  conn->handleConnection();
}


void PollPoller::handleMessageEvent(int peerfd) {
  //std::cout <<"HandleMessageEvent " << peerfd << std::endl;
  TcpIterator it = lists_.find(peerfd);
  assert(it != lists_.end());
  it->second->handleMessage();
}


void PollPoller::handleCloseEvent(int i) {
  assert(i >= 0 && i < 2048);
  int peerfd = event_[i].fd;
  assert(peerfd != -1);
  //std::cout << "Close " << peerfd << std::endl;
  TcpIterator it = lists_.find(peerfd);
  assert(it != lists_.end());
  it->second->handleClose();
  lists_.erase(it);
}


void PollPoller::processConnect(const TcpConnectionPtr &conn) {
  conn->handleConnection();
}


void PollPoller::processMessage(const TcpConnectionPtr &conn) {
  conn->handleMessage();
}


void PollPoller::processClose(const TcpConnectionPtr &conn) {
  conn->handleClose();
}


void PollPoller::setConnectionCallback(const PollerCallback &cb) {
  onConnectionCallback_ = cb;
}


void PollPoller::setMessageCallback(const PollerCallback &cb) {
  onMessageCallback_ = cb;
}


void PollPoller::setCloseCallback(const PollerCallback &cb) {
  onCloseCallback_ = cb;
}
