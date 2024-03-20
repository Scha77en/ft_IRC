/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singleton.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 21:28:07 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/20 09:49:16 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include "string"
#include "../Channel/Channel.hpp"
#include <vector>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <map>

class Client;
class Channel;

class Singleton
{
	private:

		Singleton() {
		}
		Singleton(Singleton &other);
		void operator=(const Singleton &);
		
		static Singleton* singleton_;
		
		std::map<std::string, Channel * > channels;
		std::map<std::string, Client *> clients;

		Client *client_;
		Channel *channel_;
	
	public:

		static Singleton *GetInstance(void);
		~Singleton();
		
		void AddChannel(const std::string& name, Channel* channel);
		void RemoveChannel(const std::string& name);
		Channel* GetChannel(const std::string& name);

		void AddClient(const std::string& name, Client* client);
		void RemoveClient(const std::string& name);
		Client* GetClient(const std::string& name);

		void	parce_user_data(char buffer[1024]);
		void	handleTopic(char buffer[1024]);
		void	handleMode(char buffer[1024]);
		// void	handleJoin(char buffer[1024]);

		void	getName(std::string name);
		void	PrintChannels();

		void applyModeChange(char mode, bool addMode, Channel *channel);
};

#endif
