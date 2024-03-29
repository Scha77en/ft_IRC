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
		
		SYSTEM_CLIENT clients;
		SYSTEM_CHANNEL channels;

		Client *client_;
		Channel *channel_;
	
	public:

		~Database();
		static Database *GetInstance(void);
		
        
		void	PrintChannels();
		void    AddClient(const std::string& name);
        void    ParseUserInput(string data, int UserSocket);
        void    DisplayMessages(int UserSocket, string data);
        void    ListUsersChannels(string data, int UserSocket);
        void    StartCommunication(int UserSocket, string data);
        void    HandelMultiChannel(string data, int UserSocket);
		void    AddChannel(const std::string& name, Channel* channel);
        void    DisplayMessages(string data, string name, string username);
        void 	NoticeUserHasJoined(string name, string username, int UserSocket);
        void    NoticeUserLogout(string name, string username);
		void 	PRIVMessages(string data, string name, string username);

        Client* GetClient(const std::string& name);
		Channel* GetChannel(const std::string& name);

        string GetUserBySocket(int UserSocket);

		int GetUserSocket(string name);

	
		//void RemoveChannel(const std::string& name);

		//void RemoveClient(const std::string& name);
		//Client* GetClient(const std::string& name);

		//void	parce_user_data(char buffer[1024]);
		void	HandleTopic(std::string data, int UserSocket);
		void	HandleMode(std::string data, int UserSocket);
	
		std::string ExtractChannelName(std::string data);
		std::string ExtractTopic(std::string data, bool *two_dots);
		// void	handleJoin(char buffer[1024]);

		//void	getName(std::string name);

		void applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName);
};

#endif

