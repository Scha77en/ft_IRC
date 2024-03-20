/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manage.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouhbi <aouhbi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 20:21:16 by aouhbi            #+#    #+#             */
/*   Updated: 2024/03/17 20:58:02 by aouhbi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGE_HPP
#define MANAGE_HPP

#include "../Server/Server.hpp"
#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include <sys/poll.h>


class Manage
{
    private:
        Channel C;
        Server server;
		struct pollfd  *fds;
        std::map <int, Client> lstClient;
    public:
        Manage();
        Manage( char *port, char *pass );
        ~Manage();
        void loop();
        void acceptClient();
        void initFds();
        int getFreeIdx();
        void handleClient(int index);
        void deleteClient(int idx);

};

#endif