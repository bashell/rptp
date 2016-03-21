#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "InetAddress.h"

#define ERR_EXIT(m) \
  do { \
    perror(m);\
    exit(EXIT_FAILURE);\
  } while(0)


InetAddress::InetAddress(uint16_t port) {
  ::memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = ::htons(port);
  addr_.sin_addr.s_addr = INADDR_ANY;
}


InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr) {

}


void InetAddress::setSockAddrInet(const struct sockaddr_in &addr) {
  addr_ = addr;
}


const struct sockaddr_in* InetAddress::getSockAddrInet() const {
  return &addr_;
}


uint32_t InetAddress::ipNetEndian() const {
  return addr_.sin_addr.s_addr;
}


uint16_t InetAddress::portNetEndian() const {
  return addr_.sin_port;
}


std::string InetAddress::toIp() const {
  // inet_ntoa: struct in_addr sin_addr --->  "192.168.1.1"
  return std::string(::inet_ntoa(addr_.sin_addr));
}


uint16_t InetAddress::toPort() const {
  return ::ntohs(addr_.sin_port);
}


InetAddress InetAddress::getLocalAddress(int sockfd) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if(::getsockname(sockfd, (struct sockaddr*)&addr, &len) == -1)  // 获得绑定套接字的本机地址
    ERR_EXIT("getsockname");
  return InetAddress(addr);
}


InetAddress InetAddress::getPeerAddress(int sockfd) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if(::getpeername(sockfd, (struct sockaddr*)&addr, &len) == -1)  // 获得绑定套接字的对等方地址
    ERR_EXIT("getpeername");
  return InetAddress(addr);
}
