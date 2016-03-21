#include <iostream>
#include <string>
#include <functional>
#include "server.h"

using namespace std::placeholders;

Server::Server(const InetAddress &addr) : server_(addr) {
  server_.setConnection(std::bind(&Server::onConnection, this, _1));
  server_.setMessage(std::bind(&Server::onMessage, this, _1));
  server_.setClose(std::bind(&Server::onClose, this, _1));
}

void Server::start() {
  server_.start();
}

void Server::onConnection(const TcpConnectionPtr &conn) {
  std::cout << "ip: " << conn->getPeerAddr().toIp() 
            << " port: " << conn->getPeerAddr().toPort() 
            << std::endl;
  conn->send("Hello, welcome!\n");
}

void Server::onMessage(const TcpConnectionPtr &conn) {
  std::string s(conn->receive());
  conn->send(s);
}

void Server::onClose(const TcpConnectionPtr &conn) {
  std::cout << "ip: " << conn->getPeerAddr().toIp()
            << " port: " << conn->getPeerAddr().toPort()
            << " close." << std::endl;
  conn->shutdown();
}


int main()
{
  Server server(InetAddress(8888));
  server.start();
  return 0;
}
