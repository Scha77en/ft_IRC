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