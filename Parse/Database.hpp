#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <iterator>
#include <cstring>
#include <strings.h>
#include "Client.hpp"
#include "Channel.hpp"
#include <map>

#define TRUE 1
#define FALSE 0

#define MAX_CL 30
#define undefine 0
#define MAX_CHANNEL 20

class Client;
class Channel;
const int xBUFFER_SIZE = 1024;

typedef std::string string;
typedef std::vector<std::string> STORE;

typedef std::map<std::string, Client* > SYSTEM_CLIENT;
typedef std::map<std::string, Channel* > SYSTEM_CHANNEL;

typedef std::vector<std::pair<std::string, std::string> > SYSTEM_KEYVAL;


class Database
{
	private:

		Database() {}

		static Database* DB;

		struct in_addr server_ip;
		
		SYSTEM_CLIENT clients;
		SYSTEM_CHANNEL channels;

		Client *client_;
		Channel *channel_;
	
	public:

		~Database();
		static Database *GetInstance(void);

		
        
		void	PrintChannels();
		void 	SetServerIP(struct in_addr host);
		void    AddClient(const std::string& name);
        void    ParseUserInput(string data, int UserSocket);
        void    DisplayMessages(int UserSocket, string data);
        void    ListUsersChannels(string data, int UserSocket);
        void    StartCommunication(int UserSocket, string data);
        void    HandelMultiChannel(string data, int UserSocket);
		void    AddChannel(std::string& name, Channel* channel);
        void    DisplayMessages(string data, string name, string username, int UserSocket);
        void 	NoticeUserHasJoined(string name, string username, int UserSocket, string IP);
        void    NoticeUserLogout(string name, string username);
		void 	PRIVMessages(string data, string name, string username);
		void 	HandelMultiPART(string data, int UserSocket);
		void 	NoticeUserPART(string ChannelName, string username, int UserSocket, string IP);

        Client* GetClient(const std::string& name);
		Channel* GetChannel(std::string name);

		string GetServerIP(void);
        string GetUserBySocket(int UserSocket);

		int GetUserSocket(string name);
};

void ERR_NORECIPIENT_411(string username, int UserSocket);
void ERR_NOSUCHNICK_401(string username, string target, int UserSocket);
void ERR_NOSUCHCHANNEL_403(string username, string target, int UserSocket);
void RPL_AWAY_301(string username, string target, string msg ,int UserSocket);
void ERR_CANNOTSENDTOCHAN_404(string username, string target, int UserSocket);
void ERR_NOTONCHANNEL_442(string username, int UserSocket, string ChannelName);

#endif

