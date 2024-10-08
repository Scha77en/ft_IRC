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
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <map>


// ------------------ TOPIC RPL AND ERR MESSAGES ------------------
// #define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " #" +channelname + " :" + topic + "\r\n")
#define RPL_TOPIC(client, channel, topic) ":irc.1337.com 332 " + client + " " + channel + " " + topic + "\r\n"
#define RPL_NOTOPIC(client, channel) ":irc.1337.com 331 " + client + " " + channel + " No topic is set\r\n"
#define ERR_NEEDMOREPARAMS(client, command) ":irc.1337.com 461 " + client + " " + command + " :Not enough parameters\n"
#define ERR_NOSUCHCHANNEL(client, channel) ":irc.1337.com 403 " + client + " " + channel + " :No such channel\n"
#define ERR_NOTONCHANNEL(client, channel) ":irc.1337.com 442 " + client + " " + channel + " :You're not on that channel\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) ":irc.1337.com 482 " + client + " " + channel + " :You're not channel operator\n"

// ------------------ MODE RPL AND ERR MESSAGES ------------------

#define ERR_CHANNELISFULL(client, channel) ":irc.1337.com 471 " + client + " " + channel + " :Cannot join channel (+l)\n"
#define ERR_INVITEONLYCHAN(client, channel) ":irc.1337.com 473 " + client + " " + channel + " :Cannot join channel (+i)\n"
#define ERR_BADCHANNELKEY(client, channel) ":irc.1337.com 475 " + client + " " + channel + " :Cannot join channel (+k)\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) ":irc.1337.com 482 " + client + " " + channel + " :You're not channel operator\n"
#define RPL_CHANNELMODEIS(client, channel, mode) ":irc.1337.com 324 " + client + " " + channel + " " + mode + "\n"
#define ERR_UNKNOWNMODE(client, mode) ":irc.1337.com 472 " + client + " " + mode + " :is unknown mode char to me\n"
#define ERR_INVALIDMODEPARAM_O(client, channel, mode) ":irc.1337.com 696 " + client + " " + channel + " " + mode + " :You must specify a target for the operator mode (+/-)o <target>.\n"
#define ERR_INVALIDMODEPARAM_K(client, channel, mode) ":irc.1337.com 696 " + client + " " + channel + " " + mode + " :You must specify a key for the key mode (+/-)k <key>.\n"
#define ERR_INVALIDMODEPARAM_L(client, channel, mode) ":irc.1337.com 696 " + client + " " + channel + " " + mode + " :You must specify a limit for the limit mode (+/-)l <limit>.\n"
#define ERR_NOSUCHNICK_(client, target) ":irc.1337.com 401 " + client + " " + target + " :No such nick/channel\n"
#define ERR_USERNOTINCHANNEL(client, target, channel) ":irc.1337.com 441 " + client + " " + target + " " + channel + " :They aren't on that channel\n"

// ------------------ INVITE RPL AND ERR MESSAGES ------------------

#define RPL_INVITING(client, target, channel) ":irc.1337.com 341 " + client + " " + target + " " + channel + "\n"
#define ERR_USERONCHANNEL(client, user, channel) ":irc.1337.com 443 " + client + " " + user + " " + channel + " :is already on channel\n"

// ------------------ GENERAL DEFINES ------------------
#define TRUE 1
#define FALSE 0

#define MAX_CL 30
#define undefine 0
#define MAX_CHANNEL 20

class Client;
class Channel;
class server;

const int xBUFFER_SIZE = 1024;

typedef std::vector<std::string> STORE;

typedef std::map<std::string, Client* > SYSTEM_CLIENT;
typedef std::map<std::string, Channel* > SYSTEM_CHANNEL;

typedef std::vector<std::pair<std::string, std::string> > SYSTEM_KEYVAL;


class Database
{
	private:

		Database() {}

		static Database* DB;

		int				server_socket;
		struct in_addr	server_ip;
		
		SYSTEM_CLIENT	clients;
		SYSTEM_CHANNEL	channels;

		Client		*client_;
		Channel		*channel_;
		server		*server_;
	
	public:

		~Database();
		static Database *GetInstance(void);

		
        
		void	PrintChannels();
		void 	SetServerIP(struct in_addr host);
		void    AddClient(Client* client);
        void    ParseUserInput(std::string data, int UserSocket);
        void    DisplayMessages(int UserSocket, std::string data);
        void    ListUsersChannels(std::string data, int UserSocket);
        void    StartCommunication(int UserSocket, std::string data);


        void    HandelMultiChannel(std::string data, int UserSocket);
		void 	HandelMultiPART(std::string data, int UserSocket);
		void 	HandelKick(std::string data, int UserSocket);


        void 	NoticeUserHasJoined(std::string name, std::string username, int UserSocket, std::string IP);
		void 	NoticeUserPART(std::string ChannelName, std::string username, int UserSocket, std::string IP, std::string msg);
		void 	NoticeUserKICK(std::string ChannelName, std::string username, std::string IP, std::string target, std::string msg);

		void    AddChannel(const std::string& name, Channel* channel);
        void    DisplayMessages(std::string data, std::string name, std::string username, int UserSocket);
        void    NoticeUserLogout(std::string name, std::string username);
		void 	PRIVMessages(std::string data, std::string name, std::string username);

        Client* GetClient(const std::string& name);
		Channel* GetChannel(const std::string& name);

		std::string GetServerIP(void);
        std::string GetUserBySocket(int UserSocket);

		int GetUserSocket(std::string name);

	
		void	HandleTopic(std::string data, int UserSocket);
		void	HandleMode(std::string data, int UserSocket);
		void	HandleInvite(std::string data, int UserSocket);
		void	HandleBot(std::string data, int UserSocket);

		std::string ExtractChannelName(std::string data);
		std::string ExtractTopic(std::string data, bool *two_dots);

		void applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName, std::vector<std::string> &m_args);
		bool DoesChannelExist(std::string channelName);
		bool IsUserInChannel(std::string channelName, std::string UserName);

		void RemoveClient(int fd);

		void SetServerSocket(int socket);
		void CloseServer(void);
		void printusers();

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
		void	ERR_441_USERNOTINCHANNEL(std::string UserName, std::string target, std::string channelName, int UserSocket);
		void	RPL_341_INVITING(std::string UserName, std::string target, std::string channel, int UserSocket);
		void	ERR_443_USERONCHANNEL(std::string UserName, std::string target, std::string channel, int UserSocket);
		

};

// Reply.cpp
void ERR_NORECIPIENT_411(std::string username, int UserSocket);
void ERR_NOTEXTTOSEND_412(std::string username, int UserSocket);
void ERR_NOSUCHNICK_401(std::string username, std::string target, int UserSocket);
void ERR_NEEDMOREPARAMS_461(std::string name, int UserSocket, std::string username);
void ERR_NOSUCHCHANNEL_403(std::string username, std::string target, int UserSocket);
void ERR_TOOMANYCHANNELS_405(std::string name, int UserSocket, std::string username);
void ERR_UNKNOWNCOMMAND_421(std::string command, int UserSocket, std::string username);
void RPL_AWAY_301(std::string username, std::string target, std::string msg ,int UserSocket);
void ERR_CANNOTSENDTOCHAN_404(std::string username, std::string target, int UserSocket);
void ERR_NOTONCHANNEL_442(std::string username, int UserSocket, std::string ChannelName);
void ERR_CHANOPRIVSNEEDED_482(std::string username, int UserSocket, std::string ChannelName);
void ERR_BADCHANNELKEY_475(std::string name, std::string username, int UserSocket, std::string IP);
void ERR_CHANNELISFULL_471(std::string name, int UserSocket, std::string username, std::string IP);
void ERR_BANNEDFROMCHAN_474(std::string name, int UserSocket, std::string username, std::string IP);
void ERR_INVITEONLYCHAN_473(std::string name, int UserSocket, std::string username, std::string IP);

// Outils.cpp
STORE GetSendingList(std::string data, std::string &message);
STORE TokenRangeExtract(std::string data);
void	RemoveNewLine(std::string &str);
void	GetCommand(const char* buffer, std::string& command, std::string& channel);
void	RemoveCharacter(int ASCII, std::string &target);
void	ExtractTwoRnages(std::string data, std::string &ch, std::string &ky);
void	CleanInput(std::string &data, char Delimiter);
bool Protection(std::string data);
bool Potection403(std::string data);
SYSTEM_KEYVAL	parseChannels(std::string &input, int SK, std::string username);

#endif

