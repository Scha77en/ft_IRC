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
#include <utility> 

#include "Database.hpp"

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

#define MAX_CHANNEL 20
#define MAX_CL 30

const int XBUFFER_SIZE = 1024;

typedef std::string string;
typedef std::vector<std::pair<string, string> > MSG;
typedef std::map<std::string, std::vector<std::string> > CHANL;

class Channel 
{

    private:

        CHANL channels;
        
        MSG ChannelMsg[MAX_CHANNEL];

    public:

        //Channel();
        //~Channel();

        // KICK JOIN INVITE
        void handleJoin(std::string channel, std::string user);
        //void handleInvite(std::string channel, std::string user);
        //void handleKick(std::string channel, std::string user);

        string GetChannel(int index);

        void DisplayMS(string &channel, string &user);
        string SendMS(string channel, string user, int NewClientSocket);
        void DispalyInChannel(int NewClientSocket, string MSG, DataBase &info, string channel);
        void PromptMSG(string channel, string user, int NewClientSocket);

        int GetChannelIndex(const string &Ch);
        void ListUsers(std::string channel, int sockfd);
        bool AlreadyRegister(std::string channel, std::string &user);

};

#endif