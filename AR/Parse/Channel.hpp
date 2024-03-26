#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Database.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>
#include <utility> 


#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

class Client;
class Database;

typedef std::vector<std::string> Container;

class Channel 
{
	private:
		std::string _key;
		std::string _name;
		std::string _topic;

		Container _members;
		Container _admins;
		Container _invited;

		int _limit;

		bool _invite_only;
		bool _protectedTopic;

	public:

		Channel();
		~Channel();


		void handleTopic(std::string topic);
		std::string getTopic();
		void setTopic(std::string topic);

		void setUserLimit(int limit);
		int getUserLimit(void);

		void setKey(std::string key);
		std::string getKey(void);
		
		bool isInviteOnly();
		void setInviteOnly(bool invite_only);


		void setProtectedTopic(bool protectedTopic);
		bool isProtectedTopic();

		int DoesClientExist(const std::string name);
		void    BroadCastMessage(std::string broadcast);
        /*
		void parce_user_data(char buffer[1024]);
		// void	handleJoin(char buffer[1024]);

		void addMember(std::string member);
		std::string getName();
		void setName(std::string name);

		// void	setUserMode(std::string mode);
        */
};

#endif

/*
#define MAX_CHANNEL 20
#define MAX_CL 30

const int XBUFFER_SIZE = 1024;

typedef std::string string;
typedef std::vector<std::pair<string, string> > MSG;
typedef std::map<std::string, std::vector<std::string> > CHANL;

class Channel 
{

    private:

        CHANL channels;
        
        MSG ChannelMsg[MAX_CHANNEL];

    public:

        //Channel();
        //~Channel();

        // KICK JOIN INVITE
        void handleJoin(std::string channel, std::string user);
        //void handleInvite(std::string channel, std::string user);
        //void handleKick(std::string channel, std::string user);

        string GetChannel(int index);

        void DisplayMS(string &channel, string &user);
        string SendMS(string channel, string user, int NewClientSocket);
        void DispalyInChannel(int NewClientSocket, string MSG, DataBase &info, string channel);
        void PromptMSG(string channel, string user, int NewClientSocket);

        int GetChannelIndex(const string &Ch);
        void ListUsers(std::string channel, int sockfd);
        bool AlreadyRegister(std::string channel, std::string &user);

};

#endif
*/
