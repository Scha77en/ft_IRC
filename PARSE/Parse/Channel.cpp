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

std::string Channel::getTopic()
{
	return _topic;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setUserLimit(int limit)
{
	_limit = limit;
}

int Channel::getUserLimit(void)
{
	return _limit;
}

void Channel::setKey(std::string key)
{
	_key = key;
}

std::string Channel::getKey(void)
{
	return _key;
}

void Channel::setInviteOnly(bool invite_only)
{
	_invite_only = invite_only;
}

bool Channel::isInviteOnly()
{
	return _invite_only;
}

void Channel::setProtectedTopic(bool protectedTopic)
{
    std::cout << "Protected topic set to " << protectedTopic << std::endl;
	_protectedTopic = protectedTopic;
}

bool Channel::isProtectedTopic()
{
	return _protectedTopic;
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
