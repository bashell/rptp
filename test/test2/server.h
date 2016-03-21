#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include "TcpServer.h"
#include "ThreadPool.h"

class Server {
 public:
  Server(const InetAddress &addr);
  void start();
  
 private:
  Server(const Server&) = delete;
  Server &operator=(const Server&) = delete;

  void onConnection(const TcpConnectionPtr &conn);
  void onMessage(const TcpConnectionPtr &conn);
  void onClose(const TcpConnectionPtr &conn);

  void compute(const std::string &s, const TcpConnectionPtr &conn);

 private:
  TcpServer server_;
  ThreadPool pool_;
};

#endif  /* _SERVER_H_ */
