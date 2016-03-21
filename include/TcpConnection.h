#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <memory>
#include <functional>
#include <string>
#include "InetAddress.h"
#include "Socket.h"
#include "Rio.h"

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;

  TcpConnection(int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
  ~TcpConnection();

  int fd() const;
  ssize_t readn(char *usrbuf, size_t n);
  ssize_t readLine(char *usrbuf, size_t maxline);
  ssize_t writen(const char *usrbuf, size_t n);

  void send(const std::string &s);  // send包装writen
  std::string receive();
  void shutdown();

  // 设置回调函数
  void setConnectionCallback(const TcpConnectionCallback &cb);
  void setMessageCallback(const TcpConnectionCallback &cb);
  void setCloseCallback(const TcpConnectionCallback &cb);

  void handleConnection();
  void handleMessage();
  void handleClose();

  // 获取地址
  const InetAddress &getLocalAddr() const;
  const InetAddress &getPeerAddr() const;

 private:
  TcpConnection(const TcpConnection&) = delete;
  TcpConnection &operator=(const TcpConnection&) = delete;

 private:
  Socket sockfd_;   // fd
  Rio buffer_;      // 读写缓冲区

  const InetAddress localAddr_;  // local
  const InetAddress peerAddr_;   // peer
  
  TcpConnectionCallback onConnectionCallback_;  // 连接建立时的回调
  TcpConnectionCallback onMessageCallback_;     // 消息到达时的回调
  TcpConnectionCallback onCloseCallback_;       // 连接关闭时的回调
};


#endif  /* _TCP_CONNECTION_H_ */
