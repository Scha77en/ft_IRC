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

        string _ip;
        string _name;
        string _pass;
	    string _username;
	    std::vector<std::string> _channels;
	
    public:
	
	    Client();
	    ~Client();

        int GetSocket(void);
        int GetChannelID(void);

        bool GetConnection();
        bool ChannelList(string name);

        string GetClientIP(void);
        string GetUsername(void);
        string GetChannelName(int index);
        string GetName(void);
        string GetPass(void);
        
        void NewClient(int fd);
        void SetName(string name);
        void SetPass(string pass);
        void SaveChannel(std::string name);
	    void setUsername(std::string username);
        void SetConnection(bool status);
        void ActiveInChannel(std::string name);
        void ChannelLogout(std::string name);
        void NewClientIP(struct in_addr ClientIP);

};


#endif