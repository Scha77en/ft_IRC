#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

typedef std::map<std::string, std::vector<std::string> > CHANL;

class Channel 
{

    private:

        CHANL channels;

    public:

        // KICK JOIN INVITE
        void handleJoin(std::string channel, std::string user);
        //void handleInvite(std::string channel, std::string user);
        //void handleKick(std::string channel, std::string user);
        
        void ListUsers(std::string channel, int sockfd);
        bool AlreadyRegister(std::string channel, std::string &user);

};

#endif