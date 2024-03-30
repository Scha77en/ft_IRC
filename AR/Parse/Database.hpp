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


// ------------------ TOPIC RPL AND ERR MESSAGES ------------------

#define RPL_TOPIC(client, channel, topic) "irc.1337.ma 332 " + client + " #" + channel + " :" + topic + "\n"
#define RPL_NOTOPIC(client, channel) "irc.1337.ma 331 " + client + " #" + channel + " :No topic is set\n"
#define ERR_NEEDMOREPARAMS(client, command) "irc.1337.ma 461 " + client + " " + command + " :Not enough parameters\n"
#define ERR_NOSUCHCHANNEL(client, channel) "irc.1337.ma 403 " + client + " #" + channel + " :No such channel\n"
#define ERR_NOTONCHANNEL(client, channel) "irc.1337.ma 442 " + client + " #" + channel + " :You're not on that channel\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) "irc.1337.ma 482 " + client + " #" + channel + " :You're not channel operator\n"

// ------------------ MODE RPL AND ERR MESSAGES ------------------

#define ERR_CHANNELISFULL(client, channel) "irc.1337.ma 471 " + client + " #" + channel + " :Cannot join channel (+l)\n"
#define ERR_INVITEONLYCHAN(client, channel) "irc.1337.ma 473 " + client + " #" + channel + " :Cannot join channel (+i)\n"
#define ERR_BADCHANNELKEY(client, channel) "irc.1337.ma 475 " + client + " #" + channel + " :Cannot join channel (+k)\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) "irc.1337.ma 482 " + client + " #" + channel + " :You're not channel operator\n"
#define RPL_CHANNELMODEIS(client, channel, mode) "irc.1337.ma 324 " + client + " #" + channel + " " + mode + "\n"
#define ERR_UNKNOWNMODE(client, mode) "irc.1337.ma 472 " + client + " " + mode + " :is unknown mode char to me\n"
#define ERR_INVALIDMODEPARAM_O(client, channel, mode) "irc.1337.ma 696 " + client + " #" + channel + " " + mode + " :You must specify a target for the operator mode (+/-)o <target>.\n"
#define ERR_INVALIDMODEPARAM_K(client, channel, mode) "irc.1337.ma 696 " + client + " #" + channel + " " + mode + " :You must specify a key for the key mode (+/-)k <key>.\n"
#define ERR_INVALIDMODEPARAM_L(client, channel, mode) "irc.1337.ma 696 " + client + " #" + channel + " " + mode + " :You must specify a limit for the limit mode (+/-)l <limit>.\n"
#define ERR_NOSUCHNICK(client, target) "irc.1337.ma 401 " + client + " " + target + " :No such nick/channel\n"

// ------------------ INVITE RPL AND ERR MESSAGES ------------------

#define RPL_INVITING(client, user, channel) "irc.1337.ma 341 " + client + " " + user + " " + channel + "\n"
#define ERR_USERONCHANNEL(client, user, channel) "irc.1337.ma 443 " + client + " " + user + " " + channel + " :is already on channel\n"

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
		void    AddChannel(const std::string& name, Channel* channel);
        void    DisplayMessages(string data, string name, string username, int UserSocket);
        void 	NoticeUserHasJoined(string name, string username, int UserSocket, string IP);
        void    NoticeUserLogout(string name, string username);
		void 	PRIVMessages(string data, string name, string username);

        Client* GetClient(const std::string& name);
		Channel* GetChannel(const std::string& name);

		string GetServerIP(void);
        string GetUserBySocket(int UserSocket);

		int GetUserSocket(string name);

	
		void	HandleTopic(std::string data, int UserSocket);
		void	HandleMode(std::string data, int UserSocket);

		std::string ExtractChannelName(std::string data);
		std::string ExtractTopic(std::string data, bool *two_dots);

		void applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName, std::vector<std::string> &m_args);
		bool DoesChannelExist(std::string channelName);
		bool IsUserInChannel(std::string channelName, std::string UserName);

		// ------------ ERROR AND REPLY HANDLING ------------

		void    ERR_442_NOTONCHANNEL(std::string username, std::string channelName, int UserSocket);
		void	ERR_461_NEEDMOREPARAMS(std::string username, std::string command, int UserSocket);
		void	ERR_403_NOSUCHCHANNEL(std::string username, std::string channelName, int UserSocket);
		void	ERR_482_CHANOPRIVSNEEDED(std::string username, std::string channelName, int UserSocket);
		void	RPL_NOTOPIC_331(std::string username, std::string channelName, int UserSocket);
		void	RPL_TOPIC_332(std::string username, std::string channelName, std::string topic, int UserSocket);
		void	RPL_324_CHANNELMODEIS(std::string username, std::string channelName, int UserSocket);
		void	ERR_472_UNKNOWNMODE(std::string username, char mode, int UserSocket);
		void	ERR_401_NOSUCHNICK(std::string username, std::string target, int UserSocket);

};

#endif

