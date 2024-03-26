#include "Channel.hpp"

Channel::Channel() {
    _topic = "";
    _invite_only = false;
    _protectedTopic = false;
}
Channel::~Channel() {}

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
	_protectedTopic = protectedTopic;
}

bool Channel::isProtectedTopic()
{
	return _protectedTopic;
}

/*
void Channel::ListUsers(std::string channel, int sockfd)
{
    CHANL::iterator it = channels.find(channel);
    if (it != channels.end())
    {
        int i = 0;
        std::stringstream buffer;   
        std::vector<std::string>::iterator Users;
        for (Users = it->second.begin(); Users != it->second.end();Users++)
            buffer << RED << "CHANNEL : " << channel << GREEN << " [ USER N" << i++ << "] " << BLUE << "@" << *Users << RESET << std::endl;
        std::string output = buffer.str();
        send(sockfd, output.c_str(), output.length(), 0);
    }
}

void Channel::handleJoin(std::string channel, std::string user) 
{
    CHANL::iterator it = channels.find(channel);
    if (it != channels.end()) 
    {
        (*it).second.push_back(user);
        std::cout << user << " joined " << channel << std::endl;
    } 
    else 
    {
        channels[channel].push_back(user);
        std::cout << user << " created and joined " << channel << std::endl;
    }
}

string Channel::GetChannel(int index)
{
    int count = 0;
    CHANL::iterator it;

    for (it = channels.begin(); it != channels.end(); ++it) 
    {
        if (count == index)
            return (it->first);
        ++count;
    }
    return ("");
}

int Channel::GetChannelIndex(const string &Ch)
{
    int index = 0;
    CHANL::iterator it;

    for (it = channels.begin(); it != channels.end(); ++it) 
    {
        if (it->first == Ch)
            return index;
        ++index;
    }
    return -1;
}

void removeNewlines(std::string &str) 
{
    size_t pos = 0;
    while ((pos = str.find('\n', pos)) != std::string::npos)
        str.erase(pos, 1);
}

void Channel::DispalyInChannel(int NewClientSocket, string MSG, DataBase &info, string channel)
{
    CHANL::iterator it = channels.find(channel);

    if (it != channels.end())
    {
        string output;
        std::stringstream Respond;   
        std::vector<std::string>::iterator Users;

        for (Users = it->second.begin(); Users != it->second.end();Users++)
        {
            int ClientSK = info.UserSocket(*Users);

            if (ClientSK != -1)
            {
                Respond.str("");
                string user = info.FindUser(NewClientSocket);
                removeNewlines(user);
                Respond << GREEN << "@" +  user << BLUE << " : "  << MSG << RESET;
                output = Respond.str();
                //removeNewlines(output); 
                send(ClientSK, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Channel::PromptMSG(string channel, string user, int NewClientSocket)
{
    std::string output;
    std::stringstream Respond;

    Respond.str("");
    Respond << BLUE << "(" + channel + ") " << GREEN << "@" + user << BLUE << " - [Message] : " << RESET;
    output = Respond.str();
    removeNewlines(output); 
    send(NewClientSocket, output.c_str(), output.length(), 0);
}

string Channel::SendMS(string channel, string user, int NewClientSocket)
{

    int ReadByte;
    char BUFFER[XBUFFER_SIZE];

    ReadByte = recv(NewClientSocket, BUFFER, XBUFFER_SIZE, 0);
    BUFFER[ReadByte] = '\0';
    std::string MSG(BUFFER, ReadByte);
    ChannelMsg[GetChannelIndex(channel)].push_back(std::make_pair("@" + user, MSG));
    return (MSG);
}
*/