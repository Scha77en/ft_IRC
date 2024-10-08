#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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

class Channel 
{
	private:
		std::string _key;
		std::string _symbol;
		std::string _name;
		std::string _topic;

		Container _members;
		Container _admins;
		Container _invited;
		Container _Banned;

		int _limit;

		bool _invite_only;
		bool _protectedTopic;

	public:

		~Channel();
		Channel(std::string name, std::string key);
	
		int GetLimit(void);
		int CountMembers(void);
		int CountInvited(void);
		int CountAdmins(void);
		int UserCategory(std::string username);

		bool isInviteOnly();
		bool FirstCreation(void);
		bool UserIsBanned(std::string username);

		std::string GetSecretKey();
		std::string ChannelName(void);
		std::string GetSymbol(void);
		std::string UserPrivillage(std::string username);

		void BanMember(std::string username);
		void SetLimit(int limited_to);
		void SetSymbol(std::string status);
		void addMember(std::string member);
		void addAdmin(std::string member);
		void MembertoAdmin(std::string member);
		void UsersInChannel(int Sokect, std::string username, std::string IP);
		void setInviteOnly(bool invite_only);
		void PartFromChannels(std::string member);

		void	AddInvited(std::string target);

		std::string getTopic();
		void setTopic(std::string topic);

		bool setUserLimit(std::vector<std::string> &m_args, std::string UserName, bool addMode);
		int getUserLimit(void);

		bool setKey(std::vector<std::string> &m_args, bool addMode, std::string UserName);
		std::string getKey(void);

		void setProtectedTopic(bool protectedTopic);
		bool isProtectedTopic();

		int DoesClientExist(const std::string name);
		void    BroadCastMessage(std::string broadcast);

		bool	SetOperator(std::string name, bool Mode, std::vector<std::string> &m_args);

		std::string GetModes(void);

};

#endif
