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

class Client
{
    private:

        int socket;
        int channelID;
        int limited_channels;

        bool online;
        bool Auth;

        std::string _ip;
        std::string _username;
        std::string _realname;
        std::string _pass;
	    std::string _nickname;
	    std::vector<std::string> _channels;
        std::string bufferClient;
	
    public:
	
	    Client();
	    ~Client();

        void SetBufferClient(std::string buffer);
        std::string GetBufferClient(void);
        void clearBufferClient(void);

        int GetSocket(void);
        int GetChannelID(void);

        bool GetConnection();
        bool ChannelList(std::string name);
        bool LogoutFromAllChannels();
        bool LimitedChannels(void);
        bool DeleteChannel(std::string name);

        std::string GetClientIP(void);
        std::string GetUsername(void);
        std::string GetNickname(void);
        std::string GetRealName(void);
        std::string GetChannelName(int index);
        std::string GetPass(void);
        bool GetAuth(void);
        
        std::string GetName(void);
        std::vector<std::string> GetChannels(void);

        void SetAuth(bool status);
        void NewClient(int fd);
        void SetUserName(std::string username);
        void SetNickName(std::string nickname);
        void SetRealName(std::string realname);
        void SetPass(std::string pass);
        void SaveChannel(std::string name);
        void SetConnection(bool status);
        void ActiveInChannel(std::string name);
        void ChannelLogout(std::string name);
        void NewClientIP(struct in_addr ClientIP);

        void Setlogtime_client(tm *dt);
        tm *Getlogtime_client();
        void Setstart_time_client(time_t *dt);
        time_t *Getstart_time_client();

        std::vector<std::string> GetAllChannels(void);

};


#endif