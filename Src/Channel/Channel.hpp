/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 19:30:00 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/20 09:55:18 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../singleton/singleton.hpp"
#include <iostream>
#include <string>
#include <vector>

class Channel {
	private:
		std::string _name;
		std::vector<std::string> _members;
		std::vector<std::string> _admins;
		std::vector<std::string> _invited;
		std::string _topic;
		int _limit;
		bool _invite_only;
		bool _protectedTopic;
		std::string _key;

	public:
		Channel();
		~Channel();

		void parce_user_data(char buffer[1024]);
		void handleTopic(char buffer[1024]);
		// void	handleJoin(char buffer[1024]);

		void addMember(std::string member);
		std::string getTopic();
		void setTopic(std::string topic);
		std::string getName();
		void setName(std::string name);

		void setUserLimit(int limit);
		int getUserLimit(void);
		// void	setUserMode(std::string mode);
		void setKey(std::string key);
		std::string getKey(void);
		void setInviteOnly(bool invite_only);
		bool isInviteOnly();
		void setProtectedTopic(bool protectedTopic);
		bool isProtectedTopic();

		// void	removeMember(std::string member);
		// void	addAdmin(std::string admin);
		// void	removeAdmin(std::string admin);
		// void	addInvited(std::string invited);
		// void	removeInvited(std::string invited);
		// std::vector<std::string> getMembers();
		// std::vector<std::string> getAdmins();
		// std::vector<std::string> getInvited();
		// // std::vector<std::string> getBanned();
		// // std::vector<std::string> getMuted();
		// // std::vector<std::string> getOperators();
		// // std::vector<std::string> getVoices();
		// std::string	getMode();
		// void	setMode(std::string mode);
		// std::string	getKey();
		// void	setKey(std::string key);
		// std::string	getOwner();
		// void	setOwner(std::string owner);
		// std::string	getLimit();
		// void	setLimit(std::string limit);
		// std::string	getBanMask();
		// void	setBanMask(std::string banMask);
		// std::string	getInviteMask();
		// void	setInviteMask(std::string inviteMask);
		// std::string	getExceptMask();
		// void	setExceptMask(std::string exceptMask);
		// std::string	getInviteExceptionMask();
		// void	setInviteExceptionMask(std::string inviteExceptionMask);
		// std::string	getKeyExceptionMask();
		// void	setKeyExceptionMask(std::string keyExceptionMask);
		// std::string	getLimitExceptionMask();
		// void	setLimitExceptionMask(std::string limitExceptionMask);
		// std::string	getModeExceptionMask();
		// void	setModeExceptionMask(std::string modeExceptionMask);
		// std::string	getTopicExceptionMask();
		// void	setTopicExceptionMask(std::string topicExceptionMask);
		// std::string	getUserMode(std::string user);
		// void	setUserMode(std::string user, std::string mode);
		// void	removeUserMode(std::string user, std::string mode);
		// void	addUserMode(std::string user, std::string mode);
		// void	removeUser(std::string user);
		// void	addUser(std::string user);
};

#endif