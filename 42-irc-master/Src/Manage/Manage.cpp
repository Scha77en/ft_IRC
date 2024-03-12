
#include "Manage.hpp"
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define MAX_CLIENTS SOMAXCONN

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

Manage::Manage(void) {}

void Manage::initFds()
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
  }
}

int Manage::getFreeIdx()
{
  int i;
  for (i = 1; i < MAX_CLIENTS; i++)
  {
    if (fds[i].fd == -1)
      return i;
  }
  if (i == MAX_CLIENTS)
  {
    std::cerr << "bhady si 9awd kolchi 3amer" << std::endl;
  }
  return -1;
}

Manage::~Manage()
{
  delete[] fds;
}
Manage::Manage(char *port, char *pass)
{
  fds = new pollfd[MAX_CLIENTS];
  bzero(fds, sizeof(pollfd));
  server.initServer(std::atoi(port), pass);
  server.bindServer();
  initFds();
  fds[0].fd = server.getFd();
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  std::cout << "------> |||" << fds << std::endl;
}

void Manage::loop()
{
  while (true)
  {
    int status = poll(fds, MAX_CLIENTS, -1);
    if (status < 0)
      std::cerr << "Error: Poll" << std::endl;
    {
      if (fds[0].revents & POLLIN)
      {
        acceptClient();
      }
      for (int i = 1; i < MAX_CLIENTS; i++)
      {
        if (fds[i].revents & POLLIN)
        {
          handleClient(i);
          std::cout << GREEN << "[Client Recieved Data !!]" << RESET << std::endl;
        }
        if (fds[i].revents & POLLHUP)
        {
          std::cout << "hnaaa!!" << std::endl;
          deleteClient(i);
        }
      }
    }
  }
}

void Manage::acceptClient()
{
  int sock = accept(server.getFd(), (struct sockaddr *)0, (socklen_t *)0);
  if (sock < 0)
    return (std::cerr << "Failed to accept Client !!" << std::endl, (void)0);
  Client tmp;
  tmp.fd = sock;
  lstClient[sock] = tmp;
  int idx = getFreeIdx();
  fds[idx].fd = sock;
  fds[idx].revents = 0;
  fds[idx].events = POLLIN;
  std::cout << GREEN << "[Client Joined !!]" << RESET << std::endl;
}

void Manage::handleClient(int index) {
  std::cout << PURPLE << "handling client with [index] " << index << " [fd] " << fds[index].fd << RESET << std::endl;
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t re = recv(fds[index].fd, buffer, sizeof(buffer) - 1, 0);
  write(1, buffer, re);
  if (re == -1) 
    return (std::perror("Error reading from client"), (void)0);
  if(re == 0)
    return(std::cout << GREEN << "[Client Joined !!]" << RESET << std::endl, deleteClient(index), (void)0);
  std::cout << GREEN "Buffer = " << buffer << std::endl;
  // get_user_data();
}

void Manage::deleteClient(int idx)
{
  if (lstClient.find(fds[idx].fd) != lstClient.end())
    lstClient.erase(fds[idx].fd);
  std::cout << GREEN << "[Client Deleted !!]" << RESET << std::endl;
  close(fds[idx].fd);
  fds[idx].fd = -1;
  fds[idx].events = POLLNVAL;
  fds[idx].revents = POLLNVAL;
}
