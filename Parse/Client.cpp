#include "Client.hpp"

Client::Client() 
{
    this->online = false;
    this->channelID = -1;
    this->socket = 0;
    this->limited_channels = 0;
}

Client::~Client() {}

void Client::NewClient(int fd) 
{
	this->socket = fd;
}

void Client::SetNickName(string name)
{
    this->_nickname = name;
}

void Client::SetRealName(std::string realname)
{
    this->_realname = realname;
}

void Client::SetUserName(string name)
{
    this->_username = name;
}

void Client::SetPass(string pass)
{
    this->_pass = pass;
}

string Client::GetUsername(void)
{
    return this->_username;
}

string Client::GetNickname(void)
{
    return this->_nickname;
}

std::string Client::GetRealName(void)
{
    return this->_realname;
}

string Client::GetPass(void)
{
    return this->_pass;
}

bool Client::GetAuth(void)
{
    return this->Auth;
}

void Client::SetAuth(bool status)
{
    this->Auth = status;
}

bool Client::LimitedChannels(void)
{
    if (limited_channels <= 0)
        return (undefine);
    return ((int)_channels.size() == limited_channels);
}

bool Client::LogoutFromAllChannels()
{
    bool has_channels = undefine;
    std::vector<std::string>::iterator it;

    for (it = _channels.begin(); it != _channels.end();++it)
    {
        has_channels = 1;
        _channels.erase(it);
    }
    return (has_channels);
}

std::vector<std::string> Client::GetAllChannels(void)
{
    return _channels;
}


void Client::NewClientIP(struct in_addr ClientIP)
{
    string IP(inet_ntoa(ClientIP));
    this->_ip = IP;
}

string Client::GetClientIP(void)
{
    std::cout << "IP ===> " << this->_ip << std::endl;
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

bool Client::DeleteChannel(string name)
{
    std::vector<std::string>::iterator it;

    for (it = _channels.begin(); it != _channels.end();++it)
    {
        if (*it == name)
        {
            _channels.erase(it);
            return (1);
        }
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
    return ("");
}

void Client::ChannelLogout(std::string name)
{
    std::vector<std::string>::iterator it = std::find(_channels.begin(), _channels.end(), name);

    if (it != _channels.end()) 
        _channels.erase(it);
}