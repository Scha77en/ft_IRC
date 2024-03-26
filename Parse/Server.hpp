#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>

#include "Channel.hpp"
#include "Database.hpp"

#define undefine 0
#define TRUE 1
#define FALSE 0

#define MAX_CLIENTS 30

const int PORT = 5554;
const int BUFFER_SIZE = 1024;

typedef std::string string;

class Server 
{
    private:

        Server() {}

        static Server* instance;
        struct sockaddr_in server_addr;
        struct pollfd fds[MAX_CLIENTS];

        socklen_t addrlen;
        
        int server_socket;
    

        Database *info;

    public:

        void ServerAddr();
        void StartSession();
        void RemoveNewLines(string &str);
        
        bool ServerCreate();
        bool ProcessClient();
        bool ProccessUserData(int NewClientSocket);

        static Server* getInstance();
};

#endif
