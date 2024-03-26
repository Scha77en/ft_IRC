#include "Channel.hpp"

Channel::~Channel() {}

Channel::Channel(std::string name, std::string key) 
{
	this->_name = name;
    this->_key = key;
}

void Channel::addMember(std::string member)
{
    _members.push_back(member);
}

std::string Channel::ChannelName(void)
{
    return this->_name;
}

void Channel::MembertoAdmin(std::string member)
{
    Container::iterator it = std::find(_members.begin(), _members.end(), member);

    if (it != _members.end()) 
    {
        _admins.push_back(*it);
        _members.erase(it);
    }
}

std::string Channel::GetSecretKey()
{
    return this->_key;
}

void Channel::addAdmin(std::string member)
{
    _admins.push_back(member);
}

bool Channel::FirstCreation(void)
{
    if (_members.size() == 0 && _admins.size() == 0)
        return (1);
    return (0);
}

void Channel::UsersInChannel(int Sokect, std::string username)
{
    std::string output;
    std::stringstream Respond;

    Respond << RESET << "(353) " << username + " = " << "#" + ChannelName() + " :";
    for (size_t i = 0;i < _members.size(); i++)
        Respond << BLUE << " " + _members[i] << RESET;
    for (size_t i = 0;i < _admins.size(); i++)
        Respond << GREEN << " @" + _admins[i] << RESET;
    Respond << std::endl;
    output = Respond.str();
    send(Sokect, output.c_str(), output.length(), 0);
}
