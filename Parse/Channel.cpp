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

bool Channel::AlreadyRegister(std::string channel, std::string &user)
{
    CHANL::iterator it = channels.find(channel);
    std::vector<std::string>::iterator Users;
    for (Users = it->second.begin(); Users != it->second.end();Users++)
    {
        bool found = false;
        std::string Target(*Users);
        for (size_t i = 0; i < Target.length() && !found; i++)
        {
            if (Target[i] != user[i])
                found = true;
        }
        if (found)
            return (EXIT_SUCCESS);
    }
    return (EXIT_FAILURE);
}


void Channel::handleJoin(std::string channel, std::string user) 
{
    CHANL::iterator it = channels.find(channel);
    if (it != channels.end()) 
    {
        if (AlreadyRegister(channel, user))
            return ;
        (*it).second.push_back(user);
        std::cout << user << " joined " << channel << std::endl;
    } 
    else 
    {
        channels[channel].push_back(user);
        std::cout << user << " created and joined " << channel << std::endl;
    }
}