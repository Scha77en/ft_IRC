#include "Server.hpp"
#include "Channel.hpp"
#include <csignal>

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./parse [PORT] [PASSWORD]" << std::endl;
        return 1;
    }
    try
    {
        Server *server = Server::getInstance(av[1], av[2]);
        server->ServerCreate();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
