#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <memory>
#include "Socket.h"
#include "TcpConnection.h"
#include "PollPoller.h"


class TcpServer {
 public:
  /*
   * TcpConnectionPtr = std::shared_ptr<TcpConnection>
   * TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>
   */
  using TcpCallback = TcpConnection::TcpConnectionCallback;

  explicit TcpServer(const InetAddress &addr);  // 抑制构造函数隐式转换

  void setConnection(const TcpCallback &cb);
  void setMessage(const TcpCallback &cb);
  void setClose(const TcpCallback &cb);

  void start();

 private:
  TcpServer(const TcpServer&) = delete;
  TcpServer &operator=(const TcpServer&) = delete;

 private:
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<PollPoller> poller_;

  TcpCallback onConnect_;
  TcpCallback onMessage_;
  TcpCallback onClose_;
};


#endif  /* _TCP_SERVER_H_ */
