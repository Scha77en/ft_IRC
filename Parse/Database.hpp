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
// #define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " #" +channelname + " :" + topic + "\r\n")
#define RPL_TOPIC(client, channel, topic) ":irc.1337.com 332 " + client + " " + channel + " " + topic + "\r\n"
#define RPL_NOTOPIC(client, channel) ":irc.1337.com 331 " + client + " " + channel + " No topic is set\r\n"
#define ERR_NEEDMOREPARAMS(client, command) ":irc.1337.com 461 " + client + " " + command + " Not enough parameters\n"
#define ERR_NOSUCHCHANNEL(client, channel) ":irc.1337.com 403 " + client + " " + channel + " No such channel\n"
#define ERR_NOTONCHANNEL(client, channel) ":irc.1337.com 442 " + client + " " + channel + " You're not on that channel\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) ":irc.1337.com 482 " + client + " " + channel + " You're not channel operator\n"

// ------------------ MODE RPL AND ERR MESSAGES ------------------

#define ERR_CHANNELISFULL(client, channel) ":irc.1337.com 471 " + client + " " + channel + " :Cannot join channel (+l)\n"
#define ERR_INVITEONLYCHAN(client, channel) ":irc.1337.com 473 " + client + " " + channel + " :Cannot join channel (+i)\n"
#define ERR_BADCHANNELKEY(client, channel) ":irc.1337.com 475 " + client + " " + channel + " :Cannot join channel (+k)\n"
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

#define JOIN_SUCC(nickname, username, client_ip, channel) ":" + (nickname) + "!~" + (username) + "@" + (client_ip) + " JOIN " + (channel)


#define TRUE 1
#define FALSE 0

#define MAX_CL 30
#define undefine 0
#define MAX_CHANNEL 20

class Client;
class Channel;
class server;
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

		// *******************************************************

		void	HandleTopic(std::string data, int UserSocket);
		void	HandleMode(std::string data, int UserSocket);
		void	HandleInvite(std::string data, int UserSocket);

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
		void	ERR_441_USERNOTINCHANNEL(std::string UserName, std::string target, std::string channelName, int UserSocket);
		void	RPL_341_INVITING(std::string UserName, std::string target, std::string channel, int UserSocket);
		void	ERR_443_USERONCHANNEL(std::string UserName, std::string target, std::string channel, int UserSocket);
		void	RPL_JOINSUCCESS(std::string nickname, std::string username, std::string client_ip, std::string channel);
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