#include <exception>
#include <iostream>
#include <stdexcept>
#include "./Manage/Manage.hpp"

int main(int ac, char **av) { // check the port

  try {
    if (ac != 3)
      throw std::runtime_error("Usage : /ircserv <port> <password>");
    Manage  inst_( av[1], av[2] );
    inst_.loop();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}