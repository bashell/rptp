#include <iostream>
#include <utility>  // pair
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "PollPoller.h"

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


PollPoller::PollPoller(int listenfd) : listenfd_(listenfd) {
  for(int i = 0; i != POLL_EVENT_SIZE; ++i)
    event_[i].fd = -1;
  event_[0].fd = listenfd_;
  event_[0].events = POLLIN;  // events of interest on fd
  maxi_ = nready_ = 0;
}


PollPoller::~PollPoller() {
  ::close(listenfd_); 
}


void PollPoller::poll() {
  int ret;
  do {
    ret = ::poll(event_, maxi_ + 1, 10000);  // 等待10s
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
      char buf[4096];
      int nready = ::recv(peerfd, buf, sizeof(buf), MSG_PEEK);  // 查看但不取走数据
      if(nready == -1)
        ERR_EXIT("recv");
      else if(nready == 0)  // 客户fd关闭
        handleCloseEvent(i);
      else  // 处理消息事件
        handleMessageEvent(peerfd);

      --nready_;
    }
  }
}


void PollPoller::handleConnectionEvent(int peerfd) {
  // 1. 添加fd至event_
  int i;
  for(i = 1; i != POLL_EVENT_SIZE; ++i) {
    if(event_[i].fd == -1) {  // 找到第一个未用fd
      event_[i].fd = peerfd;
      event_[i].events = POLLIN;
      if(i > maxi_)
        maxi_ = i;
      break;
    }
  }
  if(i == POLL_EVENT_SIZE) 
    ERR_EXIT("Too many clients.");

  TcpConnectionPtr conn(new TcpConnection(peerfd,
                        InetAddress::getLocalAddress(peerfd),
                        InetAddress::getPeerAddress(peerfd)));
  conn->setConnectionCallback(onConnectionCallback_);
  conn->setMessageCallback(onMessageCallback_);
  conn->setCloseCallback(onCloseCallback_);

  // 2. map中新增一条
  std::pair<TcpIterator, bool> ret = itmap_.insert(std::make_pair(peerfd, conn));
  
  // 3. 调用函数
  assert(ret.second == true);
  conn->handleConnection();
}


void PollPoller::handleMessageEvent(int peerfd) {
  //std::cout <<"HandleMessageEvent " << peerfd << std::endl;
  
  // 1. 查找map
  TcpIterator it = itmap_.find(peerfd);

  // 2. 调用callback
  assert(it != itmap_.end());
  it->second->handleMessage();
}


void PollPoller::handleCloseEvent(int i) {
  // 1. 重置fd
  assert(i >= 0 && i < POLL_EVENT_SIZE);
  int peerfd = event_[i].fd;
  assert(peerfd != -1);
  event_[i].fd = -1;
  //std::cout << "Close " << peerfd << std::endl;
  
  // 2. 从map中删除fd
  TcpIterator it = itmap_.find(peerfd);
  assert(it != itmap_.end());
  it->second->handleClose();
  itmap_.erase(it);
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
