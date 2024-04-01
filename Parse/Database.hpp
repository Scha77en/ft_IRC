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
        void    ListUsersChannels(string data, int UserSocket);
		void    AddChannel(std::string& name, Channel* channel);

        void    StartCommunication(int UserSocket, string data);
        void    DisplayMessages(string data, string name, string username, int UserSocket);
		void 	PRIVMessages(string data, string name, string username);
        void    HandelMultiChannel(string data, int UserSocket);
		void 	HandelMultiPART(string data, int UserSocket);
		void 	HandelKick(string data, int UserSocket);

        void 	NoticeUserHasJoined(string name, string username, int UserSocket, string IP);
		void 	NoticeUserPART(string ChannelName, string username, int UserSocket, string IP, string msg);
		void 	NoticeUserKICK(string ChannelName, string username, string IP, string target, string msg);
		

        Client* GetClient(const std::string& name);
		Channel* GetChannel(std::string name);

		string GetServerIP(void);
        string GetUserBySocket(int UserSocket);

		int GetUserSocket(string name);
};

// Reply.cpp
void ERR_NORECIPIENT_411(string username, int UserSocket);
void ERR_NOTEXTTOSEND_412(string username, int UserSocket);
void ERR_NOSUCHNICK_401(string username, string target, int UserSocket);
void ERR_NEEDMOREPARAMS_461(string name, int UserSocket, string username);
void ERR_NOSUCHCHANNEL_403(string username, string target, int UserSocket);
void ERR_TOOMANYCHANNELS_405(string name, int UserSocket, string username);
void ERR_UNKNOWNCOMMAND_421(string command, int UserSocket, string username);
void RPL_AWAY_301(string username, string target, string msg ,int UserSocket);
void ERR_CANNOTSENDTOCHAN_404(string username, string target, int UserSocket);
void ERR_NOTONCHANNEL_442(string username, int UserSocket, string ChannelName);
void ERR_CHANOPRIVSNEEDED_482(string username, int UserSocket, string ChannelName);
void ERR_BADCHANNELKEY_475(string name, string username, int UserSocket, string IP);
void ERR_CHANNELISFULL_471(string name, int UserSocket, string username, string IP);
void ERR_BANNEDFROMCHAN_474(string name, int UserSocket, string username, string IP);
void ERR_INVITEONLYCHAN_473(string name, int UserSocket, string username, string IP);

// Outils.cpp
STORE GetSendingList(string data, string &message);
STORE TokenRangeExtract(string data);
void	RemoveNewLine(string &str);
void	GetCommand(const char* buffer, std::string& command, std::string& channel);
void	RemoveCharacter(int ASCII, string &target);
void	ExtractTwoRnages(string data, string &ch, string &ky);
void	CleanInput(std::string &data, char Delimiter);
bool Protection(string data);
bool Potection403(string data);
SYSTEM_KEYVAL	parseChannels(string &input, int SK, string username);

#endif