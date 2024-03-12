/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 19:30:00 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/12 20:11:40 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "Client.hpp"


class Channel {
	private :

		std::vector<std::string>		_members;
		std::vector<std::string>		_admins;
		std::vector<std::string>		_invited;

	public :

		Channel();
		~Channel();

		void	addMember(std::string member);
		void	removeMember(std::string member);
		void	addAdmin(std::string admin);
		void	removeAdmin(std::string admin);
		void	addInvited(std::string invited);
		void	removeInvited(std::string invited);
		std::vector<std::string> getMembers();
		std::vector<std::string> getAdmins();
		std::vector<std::string> getInvited();
		// std::vector<std::string> getBanned();
		// std::vector<std::string> getMuted();
		// std::vector<std::string> getOperators();
		// std::vector<std::string> getVoices();
		std::string	getTopic();
		void	setTopic(std::string topic);
		std::string	getMode();
		void	setMode(std::string mode);
		std::string	getKey();
		void	setKey(std::string key);
		std::string	getName();
		void	setName(std::string name);
		std::string	getOwner();
		void	setOwner(std::string owner);
		std::string	getLimit();
		void	setLimit(std::string limit);
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
		std::string	getUserMode(std::string user);
		void	setUserMode(std::string user, std::string mode);
		void	removeUserMode(std::string user, std::string mode);
		void	addUserMode(std::string user, std::string mode);
		void	removeUser(std::string user);
		void	addUser(std::string user);
		void	removeAdmin(std::string admin);
		void	addAdmin(std::string admin);
};

#endif