#include "Channel.hpp"

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
