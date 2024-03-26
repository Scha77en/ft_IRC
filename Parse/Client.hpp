#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <algorithm>
#include <map>

#define undefine 0

typedef std::string string;

class Client
{
    private:

        int socket;
        int channelID;

        bool online;

	    string _username;
	    std::vector<std::string> _channels;
	
    public:
	
	    Client();
	    ~Client();

        int GetSocket(void);
        int GetChannelID(void);

        bool GetConnection();
        bool ChannelList(string name);

        string GetUsername(void);
        string GetChannelName(int index);
        
        void NewClient(int fd);
        void SaveChannel(std::string name);
	    void setUsername(std::string username);
        void SetConnection(bool status);
        void ActiveInChannel(std::string name);
        void ChannelLogout(std::string name);

};

#endif