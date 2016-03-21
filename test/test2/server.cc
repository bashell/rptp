#include <iostream>
#include <functional>
#include "server.h"

using namespace std::placeholders;

Server::Server(const InetAddress &addr) : server_(addr), pool_(500, 10) {
  server_.setConnection(std::bind(&Server::onConnection, this, _1));
  server_.setMessage(std::bind(&Server::onMessage, this, _1));
  server_.setClose(std::bind(&Server::onClose, this, _1));
}

void Server::start() {
  pool_.start();
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
  //conn->send(s);
  pool_.addTask(std::bind(&Server::compute, this, s, conn));
}

void Server::compute(const std::string &word, const TcpConnectionPtr &conn) {
  std::string s(word);
  for(size_t ix = 0; ix != s.size(); ++ix) {
    if(isupper(s[ix]))
      s[ix] = tolower(s[ix]);
    else if(islower(s[ix]))
      s[ix] = toupper(s[ix]);
  }
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
