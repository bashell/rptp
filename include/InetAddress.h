#ifndef _INET_ADDRESS_H_
#define _INET_ADDRESS_H_

#include <netinet/in.h>
#include <string>

/*
 ** 类InetAddress对sockaddr_in做了简单的包装
 */

class InetAddress {
 public:
  explicit InetAddress(uint16_t port);  // 抑制构造函数隐式转换
  InetAddress(const struct sockaddr_in &addr);
  
  void setSockAddrInet(const struct sockaddr_in &addr);
  const struct sockaddr_in* getSockAddrInet() const;

  uint32_t ipNetEndian() const;    // sin_addr.s_addr
  uint16_t portNetEndian() const;  // sin_port

  std::string toIp() const;
  uint16_t toPort() const;

  static InetAddress getLocalAddress(int sockfd);
  static InetAddress getPeerAddress(int sockfd);

 private:
  struct sockaddr_in addr_;
};


#endif  /* _INET_ADDRESS_H_ */
