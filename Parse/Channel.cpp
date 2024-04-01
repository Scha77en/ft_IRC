#include "Channel.hpp"

Channel::~Channel() {}

Channel::Channel(std::string name, std::string key) 
{
	this->_name = name;
    this->_key = key;
    this->_limit = -1;
    this->_invite_only = 0;
}

bool Channel::isInviteOnly()
{
    return this->_invite_only;
}

void Channel::setInviteOnly(bool invite_only)
{
    this->_invite_only = invite_only;
}

bool Channel::UserIsBanned(std::string username)
{
    Container::iterator it = std::find(_Banned.begin(), _Banned.end(), username);
    if (it != _Banned.end())
        return (1);
    return (0);
}

void Channel::BanMember(std::string username)
{
    _Banned.push_back(username);
}

void Channel::addMember(std::string member)
{
    _members.push_back(member);
}

std::string Channel::ChannelName(void)
{
    return this->_name;
}

int Channel::GetLimit(void)
{
    return (this->_limit);
}

void Channel::SetLimit(int limited_to)
{
    this->_limit = limited_to;
}

int Channel::CountMembers(void)
{
    return ((int)_members.size());
}

int Channel::CountInvited(void)
{
    return ((int)_invited.size());
}

int Channel::CountAdmins(void)
{
    return ((int)_admins.size());
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

void Channel::PartFromChannels(std::string member)
{
    Container::iterator it;
    
    it = std::find(_members.begin(), _members.end(), member);
    if (it != _members.end())
        _members.erase(it);
    
    it = std::find(_admins.begin(), _admins.end(), member);
    if (it != _admins.end())
        _admins.erase(it);
    
    it = std::find(_invited.begin(), _invited.end(), member);
    if (it != _invited.end())
        _invited.erase(it);

}

int Channel::UserCategory(std::string username)
{
    Container::iterator it;
    
    it = std::find(_members.begin(), _members.end(), username);
    if (it != _members.end())
       return 1;

    it = std::find(_invited.begin(), _invited.end(), username);
    if (it != _invited.end())
        return 2;
    
    it = std::find(_admins.begin(), _admins.end(), username);
    if (it != _admins.end())
        return 3;
    
    return (0);
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

std::string Channel::GetSymbol(void)
{
    return  this->_symbol;
}

void Channel::SetSymbol(std::string status)
{
    this->_symbol = status;
}

void Channel::UsersInChannel(int Sokect, std::string username, std::string IP)
{
    std::string output;
    std::stringstream Respond;

    Respond << ":" + IP + " 353 " << username + " " + GetSymbol() + " " << ChannelName() + " :";
    for (size_t i = 0;i < _members.size(); i++)
        Respond << _members[i] + " " ;
    for (size_t i = 0;i < _admins.size(); i++)
        Respond << "@" + _admins[i] + " ";
    Respond << std::endl;
    output = Respond.str();
    send(Sokect, output.c_str(), output.length(), 0);
}
