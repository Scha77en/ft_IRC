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
#include <algorithm>
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

typedef std::vector<std::string> Container;

class Client;
class Database;

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

		~Channel();
		Channel(std::string name, std::string key);

		bool FirstCreation(void);

		std::string GetSecretKey();
		std::string ChannelName(void);
	
		void addMember(std::string member);
		void MembertoAdmin(std::string member);
		void addAdmin(std::string member);
		void UsersInChannel(int Sokect, std::string username);



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
	
		void	SetOperator(std::string name, bool Mode);	


        /*
		void parce_user_data(char buffer[1024]);
		void handleTopic(char buffer[1024]);
		// void	handleJoin(char buffer[1024]);

		std::string getTopic();
		void setTopic(std::string topic);
		std::string getName();
		void setName(std::string name);

		void setUserLimit(int limit);
		int getUserLimit(void);
		// void	setUserMode(std::string mode);
		void setKey(std::string key);
		std::string getKey(void);
		void setInviteOnly(bool invite_only);
		bool isInviteOnly();
		void setProtectedTopic(bool protectedTopic);
		bool isProtectedTopic();
        */
};

#endif
