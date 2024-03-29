#include "Server.hpp"
#include "Channel.hpp"

int main(void)
{
    Server *Service = Server::getInstance();

    Service->ServerCreate();
    
    return EXIT_SUCCESS;
}
