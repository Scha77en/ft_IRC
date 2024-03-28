#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <algorithm>
#include <cctype>
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
        struct in_addr client_ips[MAX_CLIENTS];

        socklen_t addrlen;
        
        int server_socket;

        Database *info;

    public:

        void ServerAddr();
        void StartSession();
        void RemoveNewLines(string &str);
        void RegisterClient(int NewClientSocket);

        
        bool ServerCreate();
        bool ProcessClient();
        bool GetDataInformation(int NewClientSocket, string &command, string &output);
        bool ProccessUserData(int NewClientSocket, struct in_addr ClientIP);

        static Server* getInstance();
};

#endif
