/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 20:00:49 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/20 09:54:59 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <cmath>

Channel::Channel()
{
}

Channel::~Channel()
{
}

void Channel::parce_user_data(char buffer[1024])
{
	Channel channel;
	if (std::strncmp(buffer, "TOPIC", 5) == 0)
	{
		channel.handleTopic(buffer);
	}
	else if (std::strncmp(buffer, "NAMES", 5) == 0)
	{
		
	}
	// else if (std::strncmp(buffer, "JOIN", 4) == 0)
	// {
	// 	channel.handleJoin(buffer);
	// }
}

void	Channel::handleTopic(char buffer[1024])
{
	if (buffer[6] == ' ')
	{
		_topic = buffer + 7;
	}
	else
	{
		_topic = buffer + 6;
	}
	std::cout << "Topic == " << _topic << std::endl;
}

void Channel::addMember(std::string member)
{
	_members.push_back(member);
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

// void Channel::setUserMode(std::string mode)
// {
// 	_mode = mode;
// }

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

// void	Channel::handleJoin(char buffer[1024])
// {
// 	Singleton *singleton = Singleton::GetInstance();

// 	// std::string name = "test";
// 	// std::string channel = "test1212";
// 	// Client *client = singleton->GetClient(name);
// 	// Channel *chan = singleton->GetChannel(channel);
// 	// chan->addMember(name);
// 	// client->addChannel(channel);
// 	// std::cout << "Channel = " << chan->getName() << std::endl;
// 	// std::cout << "Name = " << client->getName() << std::endl;

// }

// void Channel::removeMember(std::string member)
// {
// 	std::vector<std::string>::iterator it = std::find(_members.begin(), _members.end(), member);
// 	if (it != _members.end())
// 		_members.erase(it);
// }

// void Channel::addAdmin(std::string admin)
// {
// 	_admins.push_back(admin);
// }

// void Channel::removeAdmin(std::string admin)
// {
// 	std::vector<std::string>::iterator it = std::find(_admins.begin(), _admins.end(), admin);
// 	if (it != _admins.end())
// 		_admins.erase(it);
// }

// void Channel::addInvited(std::string invited)
// {
// 	_invited.push_back(invited);
// }

// void Channel::removeInvited(std::string invited)
// {
// 	std::vector<std::string>::iterator it = std::find(_invited.begin(), _invited.end(), invited);
// 	if (it != _invited.end())
// 		_invited.erase(it);
// }

// std::vector<std::string> Channel::getMembers()
// {
// 	return _members;
// }

// std::vector<std::string> Channel::getAdmins()
// {
// 	return _admins;
// }

// std::vector<std::string> Channel::getInvited()
// {
// 	return _invited;
// }


// std::string Channel::getMode()
// {
// 	return _mode;
// }

// void Channel::setMode(std::string mode)
// {
// 	_mode = mode;
// }

// std::string Channel::getKey()
// {
// 	return _key;
// }

// void Channel::setKey(std::string key)
// {
// 	_key = key;
// }

std::string Channel::getName()
{
	return _name;
}

void Channel::setName(std::string name)
{
	_name = name;
}

// std::string Channel::getOwner()
// {
// 	return _owner;
// }

// void Channel::setOwner(std::string owner)
// {
// 	_owner = owner;
// }

// std::string Channel::getLimit()
// {
// 	return _limit;
// }

// void Channel::setLimit(std::string limit)
// {
// 	_limit = limit;
// }

// std::string Channel::getUserMode(std::string user)
// {
// 	std::map<std::string, std::string>::iterator it = _userModes.find(user);
// 	if (it != _userModes.end())
// 		return it->second;
// 	return "";
// }

// void Channel::setUserMode(std::string user, std::string mode)
// {
// 	_userModes[user] = mode;
// }



