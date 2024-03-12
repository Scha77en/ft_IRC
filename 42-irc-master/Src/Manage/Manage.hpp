#pragma once

#include "../Server/Server.hpp"
#include "../Client/Client.hpp"
#include <map>
#include <sys/poll.h>


class Manage
{
    private:
        Server server;
		struct pollfd  *fds;
        std::map <int, Client> lstClient;
    public:
        Manage();
        Manage( char *port, char *pass );
        ~Manage();
        void loop();
        void acceptClient();
        void initFds();
        int getFreeIdx();
        void handleClient(int index);
        void deleteClient(int idx);

};