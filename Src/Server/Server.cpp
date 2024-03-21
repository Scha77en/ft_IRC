#include "Server.hpp"
#include <cstdlib>

Server::Server() {}
Server::~Server() {}

void Server::cerror(const st_ &str) {
  std::cerr << str << std::endl;
  exit(EXIT_FAILURE);
}
void Server::initServer(int port, st_ pass) {
  this->_port = port;
  this->_password = pass;
}

void Server::bindServer() {
  // std::cout << _port << std::endl;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    cerror("Server: creating socket failed");
  struct sockaddr_in addrserv;
  bzero(&addrserv, sizeof(sockaddr_in));
  addrserv.sin_family = AF_INET;
  addrserv.sin_addr.s_addr = htonl(INADDR_ANY);
  addrserv.sin_port = htons(_port);
  int optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    cerror("Error: Setting SO_REUSEADDR");
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    cerror("Error: Setting SO_REUSEPORT");
  bind(sock, (sockaddr *)&addrserv, sizeof(addrserv));
  if (listen(sock, SOMAXCONN) != 0)
    cerror("Error: Listen");
  if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
    cerror("Error: Setting socket to nonblocking");
  std::cout << "Server listening on port " << ntohs(addrserv.sin_port) << std::endl;
  this->fd = sock;
}

int Server::getFd(void) const
{
  return this->fd;
}
