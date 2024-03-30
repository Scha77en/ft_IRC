#include "Channel.hpp"

Channel::~Channel() {}

Channel::Channel(std::string name, std::string key) 
{
	this->_name = name;
    this->_key = key;
    this->_limit = -1; // -1 => not set , 2 => Only for test
    this->_invite_only = 0; // (true or false) 1 => For test only
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

    Respond << ":" + IP + " 353 " << username + " " + GetSymbol() + " " << "#" + ChannelName() + " :";
    for (size_t i = 0;i < _members.size(); i++)
        Respond << _members[i] + " " ;
    for (size_t i = 0;i < _admins.size(); i++)
        Respond << "@" + _admins[i] + " ";
    Respond << std::endl;
    output = Respond.str();
    send(Sokect, output.c_str(), output.length(), 0);
}

// ***************************************************************************

std::string Channel::getTopic()
{
    return this->_topic;
}

void Channel::setTopic(std::string topic)
{
    this->_topic = topic;
}

void Channel::setUserLimit(int limit)
{
    this->_limit = limit;
}

int Channel::getUserLimit(void)
{
    return this->_limit;
}

void Channel::setKey(std::string key)
{
    this->_key = key;
}

std::string Channel::getKey(void)
{
    return this->_key;
}

void Channel::setProtectedTopic(bool protectedTopic)
{
    this->_protectedTopic = protectedTopic;
}

bool Channel::isProtectedTopic()
{
    return this->_protectedTopic;
}

int Channel::DoesClientExist(const std::string name)
{
    if (std::find(_admins.begin(), _admins.end(), name) != _admins.end())
        return (1);
    else if (std::find(_members.begin(), _members.end(), name) != _members.end())
        return (2);
    else if (std::find(_invited.begin(), _invited.end(), name) != _invited.end())
        return (3);
    return (0);
}

void Channel::BroadCastMessage(std::string broadcast) {
    // Iterate over members
    for (Container::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        const std::string& member = *it;

        // Get the client from the database
        Client* client = Database::GetInstance()->GetClient(member);
        if (client != NULL) {
            // Get the client's socket
            int clientSocket = client->GetSocket(); // You'll need to implement this in the Client class

            // Send the message to the client
            send(clientSocket, broadcast.c_str(), broadcast.length(), 0);
        }
    }

    // Iterate over admins
    for (Container::const_iterator it = _admins.begin(); it != _admins.end(); ++it) {
        const std::string& admin = *it;

        // Get the client from the database
        Client* client = Database::GetInstance()->GetClient(admin);
        if (client != NULL) {
            // Get the client's socket
            int clientSocket = client->GetSocket(); // You'll need to implement this in the Client class

            // Send the message to the client
            send(clientSocket, broadcast.c_str(), broadcast.length(), 0);
        }
    }

    // Iterate over invited
    for (Container::const_iterator it = _invited.begin(); it != _invited.end(); ++it) {
        const std::string& invited = *it;

        // Get the client from the database
        Client* client = Database::GetInstance()->GetClient(invited);
        if (client != NULL) {
            // Get the client's socket
            int clientSocket = client->GetSocket(); // You'll need to implement this in the Client class

            // Send the message to the client
            send(clientSocket, broadcast.c_str(), broadcast.length(), 0);
        }
    }
}

void Channel::SetOperator(std::string name, bool Mode)
{
    if (Mode == 1) {
        if (std::find(_admins.begin(), _admins.end(), name) == _admins.end())
            _admins.push_back(name);
        if (std::find(_members.begin(), _members.end(), name) != _members.end())
            _members.erase(std::remove(_members.begin(), _members.end(), name), _members.end());
        if (std::find(_invited.begin(), _invited.end(), name) != _invited.end())
            _invited.erase(std::remove(_invited.begin(), _invited.end(), name), _invited.end());
    }
    else
    {
        if (std::find(_members.begin(), _members.end(), name) == _members.end())
            _members.push_back(name);
        if (std::find(_admins.begin(), _admins.end(), name) != _admins.end())
            _admins.erase(std::remove(_admins.begin(), _admins.end(), name), _admins.end());
        if (std::find(_invited.begin(), _invited.end(), name) != _invited.end())
            _invited.erase(std::remove(_invited.begin(), _invited.end(), name), _invited.end());
    }
}
