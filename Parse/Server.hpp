#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>
#include "Channel.hpp"
#include "Database.hpp"

#define MAX_CL 30
#define TRUE 1
#define FALSE 0

const int PORT = 5555;
const int BUFFER_SIZE = 1024;

class Server
{
    public:
        Server();
        ~Server();

        bool ServerCreate();
        bool ProcessClient();

        void LoginInformation(int NewClientSocket);
        void BadInput(int NewClientSocket, int ReadByte);
        void ChannelOperations(int NewClientSocket, const char *xbuffer);
        void ChannelCommunication(int NewClientSocket, string channel, string user);

    private:

        Channel Obj;
        DataBase informations;

        fd_set readfds;
        
        socklen_t addrlen;
        struct sockaddr_in server_addr;

        int server_socket;
        int client_socket[MAX_CL];

};

#endif
