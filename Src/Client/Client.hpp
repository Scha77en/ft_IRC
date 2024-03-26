#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "string"
#include <vector>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <map>

class Client
{

private:
	std::string _username;
	std::string key;
	std::string _nickname;
	std::vector<std::string> _channels;
	

public:
	int fd;
	Client();
	~Client();
	Client(int fd);
	Client(const Client &client);
	Client &operator=(const Client &client);
	void setUsername(std::string username);
	void setRealname(std::string realname);
	void setNickname(std::string nickname);
	
};

#endif