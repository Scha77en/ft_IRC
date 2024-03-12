/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 20:00:49 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/12 20:05:13 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
{
}

Channel::~Channel()
{
}

void Channel::addMember(std::string member)
{
	_members.push_back(member);
}

void Channel::removeMember(std::string member)
{
	std::vector<std::string>::iterator it = std::find(_members.begin(), _members.end(), member);
	if (it != _members.end())
		_members.erase(it);
}

void Channel::addAdmin(std::string admin)
{
	_admins.push_back(admin);
}

void Channel::removeAdmin(std::string admin)
{
	std::vector<std::string>::iterator it = std::find(_admins.begin(), _admins.end(), admin);
	if (it != _admins.end())
		_admins.erase(it);
}

void Channel::addInvited(std::string invited)
{
	_invited.push_back(invited);
}

void Channel::removeInvited(std::string invited)
{
	std::vector<std::string>::iterator it = std::find(_invited.begin(), _invited.end(), invited);
	if (it != _invited.end())
		_invited.erase(it);
}

std::vector<std::string> Channel::getMembers()
{
	return _members;
}

std::vector<std::string> Channel::getAdmins()
{
	return _admins;
}

std::vector<std::string> Channel::getInvited()
{
	return _invited;
}

std::string Channel::getTopic()
{
	return _topic;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

std::string Channel::getMode()
{
	return _mode;
}

void Channel::setMode(std::string mode)
{
	_mode = mode;
}

std::string Channel::getKey()
{
	return _key;
}

void Channel::setKey(std::string key)
{
	_key = key;
}

std::string Channel::getName()
{
	return _name;
}

void Channel::setName(std::string name)
{
	_name = name;
}

std::string Channel::getOwner()
{
	return _owner;
}

void Channel::setOwner(std::string owner)
{
	_owner = owner;
}

std::string Channel::getLimit()
{
	return _limit;
}

void Channel::setLimit(std::string limit)
{
	_limit = limit;
}

std::string Channel::getUserMode(std::string user)
{
	std::map<std::string, std::string>::iterator it = _userModes.find(user);
	if (it != _userModes.end())
		return it->second;
	return "";
}

void Channel::setUserMode(std::string user, std::string mode)
{
	_userModes[user] = mode;
}



