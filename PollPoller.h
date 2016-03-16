#ifndef _POLL_POLLER_H_
#define _POLL_POLLER_H_

#include <map>
#include <functional>
#include <poll.h>
#include "TcpConnection.h"

class PollPoller {
 public:
  using PollerCallback = TcpConnection::TcpConnectionCallback;

  explicit PollPoller(int listenfd);  // 抑制构造函数隐式转换
  ~PollPoller();

  void poll();
  void handleAccept();
  void handleData();

  void handleConnectionEvent(int peerfd);
  void handleMessageEvent(int peerfd);
  void handleCloseEvent(int index);

  void processConnect(const TcpConnectionPtr &conn);
  void processMessage(const TcpConnectionPtr &conn);
  void processClose(const TcpConnectionPtr &conn);
  
  void setConnectionCallback(const PollerCallback &cb);
  void setMessageCallback(const PollerCallback &cb);
  void setCloseCallback(const PollerCallback &cb);
  

 private:
  PollPoller(const PollPoller&) = delete;
  PollPoller &operator=(const PollPoller&) = delete;

 private:
  using TcpIterator = std::map<int, TcpConnectionPtr>::iterator;
  struct pollfd event_[2048];
  int listenfd_;
  int maxi_;
  int nready_;
  std::map<int, TcpConnectionPtr> lists_;  // fd到TcpConnection的映射

  PollerCallback onConnectionCallback_;
  PollerCallback onMessageCallback_;
  PollerCallback onCloseCallback_;
};


#endif  /* _POLL_POLLER_H_ */
