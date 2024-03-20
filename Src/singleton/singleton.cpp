/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singleton.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 21:28:04 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/20 09:57:36 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "singleton.hpp"
#include <iostream>
#include <iterator>

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

Singleton* Singleton::singleton_ = NULL;

Singleton	*Singleton::GetInstance()
{
	if (singleton_ == NULL){
		singleton_ = new Singleton();
	}
	return singleton_;
}

void Singleton::AddChannel(const std::string& name, Channel* channel)
{
	std::string name_ = name.substr(0, name.find('\n'));
	channels[name_] = channel;
	PrintChannels();
}

void Singleton::PrintChannels() {
    std::cout << "Channels:" << std::endl;
    for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        std::cout << it->first << std::endl; // Print the channel name (it->first)
    }
}

void Singleton::RemoveChannel(const std::string& name)
{
	channels.erase(name);
}

Channel* Singleton::GetChannel(const std::string& name)
{
	return channels[name];
}

void Singleton::AddClient(const std::string& name, Client* client)
{
	clients[name] = client;
}

void Singleton::RemoveClient(const std::string& name)
{
	clients.erase(name);
}

Client* Singleton::GetClient(const std::string& name)
{
	return clients[name];
}

Singleton::~Singleton()
{
	delete singleton_;
}

void Singleton::parce_user_data(char buffer[1024])
{
	Singleton *sing = Singleton::GetInstance();
	if (std::strncmp(buffer, "TOPIC", 5) == 0)
	{
		std::cout << "TOPIC HAS BEEN ENCOUNTERED" << std::endl;
		sing->handleTopic(buffer);
	}
	else if (std::strncmp(buffer, "JOIN", 4) == 0)
	{
		std::cout << "JOIN HAS BEEN ENCOUNTERED" << std::endl;
		sing->AddChannel(buffer + 5, new Channel());
	}
	else if (std::strncmp(buffer, "MODE", 4) == 0)
	{
		sing->handleMode(buffer);
	}
}

void Singleton::handleTopic(char buffer[1024])
{
	std::string name = buffer + 6;
	std::cout << "HANDLING TOPIC " << name << std::endl;
	std::string channelName = name.substr(0, name.find(' '));
	std::string topic = name.substr(name.find(' ') + 1);
	std::cout << "Channel Name = " << channelName << std::endl;
	std::cout << "Topic = " << topic << std::endl;
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	if (it != channels.end())
	{
		std::cout << YELLOW "Topic to set : " << topic << std::endl;
		it->second->setTopic(topic);
		std::cout << YELLOW "TOPIC IS SET = " << it->second->getTopic() << GREEN << std::endl;
	}
	else
	{
		std::cout << RED "CHANNEL NOT FOUND" << GREEN << std::endl;
	}
	// Channel *channel = channels[channelName];
	// channel->setTopic(topic);
	// std::cout << YELLOW "TOPIC IS SET = " << channel->getTopic() << GREEN << std::endl;
}

void	Singleton::handleMode(char buffer[1024])
{
	bool addMode = true;

	std::string data = buffer + 5;
	std::string channelName = data.substr(0, data.find(' '));
	std::string mode = data.substr(data.find(' ') + 1);
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
	for (size_t i = 0; i < mode.size(); i++) {
		char c = mode[i];
		if (c == '+' || c == '-') {
			addMode = (c == '+');
		}
		else {
			applyModeChange(c, addMode, it->second);
		}
	}
}

void	Singleton::applyModeChange(char mode, bool addMode, Channel *channel)
{
	switch (mode) {
		case 'i':
			channel->setInviteOnly(addMode);
			break;
		case 't':
			channel->setProtectedTopic(addMode);
			break;
		case 'l':
			channel->setUserLimit(10);
			break;
		case 'k':
			channel->setKey("test");
			break;
		case 'o':
			std::cout << "Operator" << std::endl;
			break;
		default:
			break;
	}
}

// void	Singleton::handleJoin(char buffer[1024])
// {
// 	Singleton *singleton = Singleton::GetInstance();

// 	std::string name = buffer + 5;
// 	std::string channel = buffer + 5;
// 	Client *client = singleton->GetClient(name);
// 	Channel *chan = singleton->GetChannel(channel);
// 	chan->addMember(name);
// 	// client->addChannel(channel);
// 	// std::cout << "Channel = " << chan->getName() << std::endl;
// 	// std::cout << "Name = " << client->getName() << std::endl;

// }
