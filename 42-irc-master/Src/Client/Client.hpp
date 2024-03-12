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
	std::string _realname;
	std::string _nickname;
	std::vector<std::string> _channels;
	std::map<std::string, std::string> _userModes;

public:
	int fd;
	Client();
	~Client();
};

#endif