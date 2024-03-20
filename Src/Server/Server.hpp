#pragma once

#include "string"
#include <vector>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>



#define st_ std::string

class Server
{

    private:
        int _port;
        std::string _password;
        int fd;

    public:
        Server();
        ~Server();
		void cerror(const st_ &str);
        void initServer(int port, st_ pass);
        void bindServer();
        
        int getFd(void) const;
};