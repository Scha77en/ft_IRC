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
        int limited_channels;
        tm *logtime_client;
        time_t *start_time_client;

        bool online;
        bool Auth;

        string bufferClient;
        string _ip;
        string _username;
        string _realname;
        string _pass;
	    string _nickname;
	    std::vector<std::string> _channels;
	
    public:
	
	    Client();
	    ~Client();

        int GetSocket(void);
        int GetChannelID(void);

        bool GetConnection();
        bool ChannelList(string name);
        bool LogoutFromAllChannels();
        bool LimitedChannels(void);
        bool DeleteChannel(string name);

        void Setlogtime_client(tm *dt);
        tm *Getlogtime_client();
        void Setstart_time_client(time_t *dt);
        time_t *Getstart_time_client();

        string GetBufferClient(void);
        string GetClientIP(void);
        string GetUsername(void);
        string GetNickname(void);
        std::string GetRealName(void);
        string GetChannelName(int index);
        string GetName(void);
        string GetPass(void);
        bool GetAuth(void);
        void SetAuth(bool status);
        void NewClient(int fd);
        void SetName(string name);
        void SetNickName(string name);
        void SetRealName(string name);
        void SetPass(string pass);
        void SaveChannel(std::string name);
	    void setUsername(std::string username);
        void SetConnection(bool status);
        void ActiveInChannel(std::string name);
        void ChannelLogout(std::string name);
        void NewClientIP(struct in_addr ClientIP);

        std::vector<std::string> GetAllChannels(void);

};


#endif