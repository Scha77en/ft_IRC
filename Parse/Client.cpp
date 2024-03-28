#include "Client.hpp"

Client::Client() 
{
    this->online = 0;
    this->channelID = -1;
    this->socket = 0;
}

Client::~Client() {}

void Client::NewClient(int fd) 
{
	this->socket = fd;
}

void Client::SetName(string name)
{
    this->_name = name;
}

void Client::SetPass(string pass)
{
    this->_pass = pass;
}

string Client::GetName(void)
{
    return this->_name;
}

string Client::GetPass(void)
{
    return this->_pass;
}


void Client::NewClientIP(struct in_addr ClientIP)
{
    string IP(inet_ntoa(ClientIP));
    this->_ip = IP;
}

string Client::GetClientIP(void)
{
    return (this->_ip);
}

int Client::GetSocket(void)
{
    return this->socket;
}

void Client::setUsername(std::string username) 
{
	this->_username = username;
}

void Client::SaveChannel(std::string name)
{
    this->_channels.push_back(name);
}

void Client::SetConnection(bool status)
{
    this->online = status;
}

bool Client::GetConnection()
{
    return this->online;
}

bool Client::ChannelList(string name)
{
    std::vector<std::string>::iterator it;

    for (it = _channels.begin(); it != _channels.end();++it)
    {
        if (*it == name)
            return (1);
    }
    return undefine;
}

void Client::ActiveInChannel(string name)
{
    int index = 0;
    std::vector<std::string>::iterator it;

    for (it = _channels.begin(); it != _channels.end();++it)
    {
        if (*it == name)
        {
            this->channelID = index;
            return ;
        }
        ++index;
    }
    return ;
}

int Client::GetChannelID(void)
{
    return this->channelID;
}

string Client::GetChannelName(int index)
{
    int i = 0;
    std::vector<std::string>::iterator it;
    
    for (it = _channels.begin(); it != _channels.end();++it)
    {
        if (i == index)
            return (*it);
        ++i;
    }
    return (undefine);
}

void Client::ChannelLogout(std::string name)
{
    std::vector<std::string>::iterator it = std::find(_channels.begin(), _channels.end(), name);

    if (it != _channels.end()) 
        _channels.erase(it);
}