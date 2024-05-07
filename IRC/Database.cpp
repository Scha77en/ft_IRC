#include "../Headers/Database.hpp"
#include "../Headers/Client.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Database* Database::DB = undefine;

Database	*Database::GetInstance()
{
	if (DB == undefine)
		DB = new Database();
	return DB;
}

void Database::CloseServer()
{
	if (server_socket != undefine)
		close(server_socket);
}

void Database::SetServerSocket(int socket)
{
	this->server_socket = socket;
}

void Database::AddClient(Client *client)
{
	clients.insert(std::make_pair(client->GetNickname(), client));
}

Client* Database::GetClient(const std::string& name)
{
	return clients[name];
}

void Database::SetServerIP(struct in_addr host)
{
	this->server_ip = host;
}

std::string Database::GetServerIP(void)
{
	std::string HOST(inet_ntoa(server_ip));
	return (HOST);
}

void Database::NoticeUserPART(std::string ChannelName, std::string username, int UserSocket, std::string IP, std::string msg)
{
    std::string output;

    Database *service = Database::GetInstance();
    Client *user = service->GetClient(username);
    
    if (!msg.empty())
    {
        size_t pos = msg.find(":");
        if (pos == std::string::npos)
        {
            size_t posX = msg.find(" ");
            msg = ":" + msg.substr(0, posX);
        }
        output = ":" + username + "!" + user->GetRealName() +  "@" + IP + " PART " + ChannelName + " " + msg + "\n";

    }
    else
        output = ":" + username + "!" + user->GetRealName() +  "@" + IP + " PART " + ChannelName + "\n";
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->ChannelList(ChannelName))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine)
                send(socket, output.c_str(), output.length(), 0);
        }
    }
    send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::NoticeUserKICK(std::string ChannelName, std::string username, std::string IP, std::string target, std::string msg)
{
    std::string output;

    Database *service = Database::GetInstance();
    Client *user = service->GetClient(username);

    if (msg.empty() || (msg.length() <= 2 && msg.length() > 0 && msg[0] == ':' && msg[1] == '\0'))
        output = ":" + username + "!" + user->GetNickname() +  "@" + IP + " KICK " + ChannelName + " " + target + " :" + target + "\n";
    else
    {
        size_t pos = msg.find(":");
        if (pos == std::string::npos)
        {
            size_t posX = msg.find(" ");
            msg = ":" + msg.substr(0, posX);
        }
        output = ":" + username + "!" + user->GetNickname() +  "@" + IP + " KICK " + ChannelName + " " + target + " " + msg + "\n";
    }
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->ChannelList(ChannelName))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine)
                send(socket, output.c_str(), output.length(), 0);
        }
    }
}

void Database::NoticeUserHasJoined(std::string name, std::string username, int UserSocket, std::string IP)
{
	(void)IP;
	std::string output;
	std::stringstream Respond;
	Client *User = GetClient(username);

	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->ChannelList(name))
		{
			int socket = it->second->GetSocket();
			if (socket > undefine)
			{
				Respond.str("");
				output = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " JOIN " + name + "\r\n";
				send(socket, output.c_str(), output.length(), 0);
			}
		}
	}
	Channel *channel = GetChannel(name);
	if (channel == undefine)
		return ;
	if (channel->getTopic().empty())
		RPL_NOTOPIC_331(username, name, UserSocket);
	else
		RPL_TOPIC_332(username, name, channel->getTopic(), UserSocket);
	SYSTEM_CHANNEL::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->first == name)
		{
			it->second->UsersInChannel(UserSocket, username, GetServerIP());
			break;
		}
	}
	Respond.str("");
	Respond << ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " :End of /NAMES list." << "\r\n";
	output = Respond.str();
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::NoticeUserLogout(std::string name, std::string username)
{
	std::string output;
	std::stringstream Respond;

	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->ChannelList(name))
		{
			int socket = it->second->GetSocket();
			if (socket > undefine)
			{
				Respond.str("");
				Respond << BLUE << "@" + username << RED << " is Logout From (" << name << ") " << RESET << std::endl;
				output = Respond.str();
				send(socket, output.c_str(), output.length(), 0);
			}
		}
	}
}

void Database::HandelMultiChannel(std::string data, int UserSocket)
{
    std::string args;
    SYSTEM_KEYVAL::iterator it;

    CleanInput(data, ' ');
    Database *service = Database::GetInstance();
    std::string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);
    if (Protection(data) || data.empty())
    {
        ERR_NEEDMOREPARAMS_461("JOIN",UserSocket, username);
        return ;
    }
    std::string IP = user->GetClientIP();
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
        std::string EXIST(it->first);
        if (user->LimitedChannels())
        {
            ERR_TOOMANYCHANNELS_405(EXIST, UserSocket, username);
            break;
        }
        if (user->ChannelList(EXIST))
            continue;
        Channel *channel = service->GetChannel(EXIST);
        if (channel != undefine)
        {
			if (channel->isInviteOnly())
            {
				int state = channel->DoesClientExist(username);
				if (state == 1 || state == 2) {
					ERR_443_USERONCHANNEL(EXIST, GetUserBySocket(UserSocket), username, UserSocket);
					continue;
				}
				else if (state != 3) {
                	ERR_INVITEONLYCHAN_473(EXIST, UserSocket, username, IP);
					continue;
				}
            }
            if (channel->GetSecretKey() != it->second)
            {
                ERR_BADCHANNELKEY_475(EXIST, username, UserSocket, IP);
                continue;
            }
            if (channel->GetLimit() != -1)
            {
                bool OutOfBound = channel->CountMembers() + channel->CountAdmins() + 1 > channel->GetLimit();
                if (OutOfBound)
                {
                    ERR_CHANNELISFULL_471(EXIST, UserSocket, username, IP);
                    continue;
                }
            }
            if (channel->UserIsBanned(username))
            {
                ERR_BANNEDFROMCHAN_474(EXIST, UserSocket, username, IP);
                continue;
            }
        }
        if (channel == undefine)
        {
            channel = new Channel(EXIST, it->second);
            channel->SetSymbol("=");
            service->AddChannel(EXIST, channel);
        }
        else
            channel->SetSymbol("@");
        user->SaveChannel(EXIST);
        if (channel->FirstCreation())
            channel->addAdmin(username);
        else
            channel->addMember(username);
        NoticeUserHasJoined(EXIST, username, UserSocket, IP);
    }
}

void Database::HandelMultiPART(std::string data, int UserSocket)
{
   std::string args;
    SYSTEM_KEYVAL::iterator it;

    CleanInput(data, ' ');
    Database *service = Database::GetInstance();
    std::string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    if (Protection(data) || data.empty())
    {
        ERR_NEEDMOREPARAMS_461("PART",UserSocket, username);
        return ;
    }

    std::string part1, part2;
    std::istringstream Object(data);

    std::getline(Object, part1, ' ');

    size_t pos = part1.find('#');
    if (pos == std::string::npos)
    {
        size_t posX = part1.find(' ');
        std::string NotFound = part1.substr(0, posX);
        ERR_NOSUCHCHANNEL_403(username, NotFound, UserSocket);
        return ;
    }
    std::getline(Object, part2);
    bool is_inChannel = undefine;
    std::string IP = user->GetClientIP();
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
        std::string EXIST(it->first);
		SYSTEM_CHANNEL::iterator xit;
        for (xit = service->channels.begin(); xit != service->channels.end() && !is_inChannel; ++xit)
        {
            std::string Detected(xit->first);
            if (Detected == EXIST)
            {
                is_inChannel = 1;
                xit->second->PartFromChannels(username);
            }
        }
        if (user->DeleteChannel(EXIST))
            NoticeUserPART(EXIST, username, UserSocket, IP, part2);
        else if (is_inChannel == undefine)
            ERR_NOSUCHCHANNEL_403(username, EXIST, UserSocket);
        else
            ERR_NOTONCHANNEL_442(username, UserSocket, EXIST);
        is_inChannel = undefine;
    }
}

void Database::HandelKick(std::string data, int UserSocket)
{
    CleanInput(data, ' ');

    Database *service = Database::GetInstance();
    std::string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    if (data.empty())
    {
        ERR_NEEDMOREPARAMS_461("KICK", UserSocket, username);
        return ;
    }

    std::istringstream Object(data);
    std::string part1, part2, part3;

    std::getline(Object, part1, ' ');

    if (part1.empty())
    {
        ERR_NEEDMOREPARAMS_461("KICK", UserSocket, username);
        return ;
    }
    std::getline(Object, part2, ' ');

    if (part2.empty())
    {
        ERR_NEEDMOREPARAMS_461("KICK", UserSocket, username);
        return ;
    }
    std::getline(Object, part3);

    std::string ChannelName(part1);
    std::string TrgetUsername(part2);

    bool not_found = undefine;
    std::string IP = user->GetClientIP();
    SYSTEM_CHANNEL::iterator xit;
    for (xit = service->channels.begin(); xit != service->channels.end(); ++xit)
    {
        std::string Detected(xit->first);

        if (Detected == ChannelName)
        {
            not_found = 1;
            int Privillage = xit->second->UserCategory(TrgetUsername);
            if (xit->second->UserCategory(username) != 3)
                ERR_CHANOPRIVSNEEDED_482(username, UserSocket, ChannelName);
            else if (Privillage == 3)
            {
                if (xit->second->UserCategory(username) == 3)
                {
                    Client *target = service->GetClient(TrgetUsername);
                    NoticeUserKICK(ChannelName, username, IP, TrgetUsername, part3);
                    xit->second->PartFromChannels(TrgetUsername);
                    target->DeleteChannel(ChannelName);
                }
                else
                    ERR_CHANOPRIVSNEEDED_482(username, UserSocket, ChannelName);
            }
            else if (Privillage == 2 || Privillage == 1)
            {
                Client *target = service->GetClient(TrgetUsername);
                NoticeUserKICK(ChannelName, username, IP, TrgetUsername, part3);
                xit->second->PartFromChannels(TrgetUsername);
                target->DeleteChannel(ChannelName);
            }
            else
                ERR_NOSUCHNICK_401(username, TrgetUsername, UserSocket);
            break;
        }
    }
    if (not_found == undefine)
        ERR_NOSUCHCHANNEL_403(username, ChannelName, UserSocket);
}

void Protection421(std::string command, int UserSocket, std::string username)
{
	std::string output = ":irc.1337.com 421 " + username + " " + command + " :Unknown command\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Protection403(std::string command, int UserSocket, std::string username)
{
	std::string output = ":irc.1337.com 403 " + username + " " + command + " :Unknown command\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::ParseUserInput(std::string data, int UserSocket)
{
	std::string args;
	std::string command;

	GetCommand(data.c_str(), command, args);

	Database *service = Database::GetInstance();
	std::string nickname = service->GetUserBySocket(UserSocket);


	if (command == "TOPIC" || command == "topic") {
		service->HandleTopic(data, UserSocket);
		return ;
	}
	else if (command == "INVITE" || command == "invite") {
		HandleInvite(data, UserSocket);
		return ;
	}
	else if (command == "MODE" || command == "mode") {
		service->HandleMode(data, UserSocket);
		return ;
	}
	else if (command == "BOT" || command == "bot") {
		service->HandleBot(data, UserSocket);
		return ;
	}

	CleanInput(data, ' ');
    std::stringstream cmd(data);
    cmd >> command;
    std::getline(cmd, args);
	
	if (command == "JOIN" || command == "join")
		HandelMultiChannel(args, UserSocket);
	else if (command == "PRIVMSG" || command == "privmsg")
		service->StartCommunication(UserSocket, args);
	else if (command == "PART" || command == "part")
        return service->HandelMultiPART(args, UserSocket);
	else if (command == "KICK" || command == "kick")
        service->HandelKick(args, UserSocket);
	else
		Protection421(command, UserSocket, nickname);
}

void ERR_NOSUCHNICK(std::string username, std::string target, int UserSocket)
{
	std::string output = ":irc.1337.com 401 " + username + " " + target + " :No such nick/channel\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::PRIVMessages(std::string data, std::string name, std::string username)
{
    int senderSocket = undefine;
    int socketFailed = GetUserSocket(name);

    std::string USER = "";
    std::string hostname = "";

    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        std::string CHECK(it->first);
        if (CHECK == username)
        {
           hostname = it->second->GetClientIP();
           USER = it->second->GetNickname();
           senderSocket = it->second->GetSocket();
           break;
        }
    }
    if (socketFailed == undefine)
    {
        ERR_NOSUCHNICK_401(username, name, senderSocket);
        return ;
    }
    Client *user = GetClient(username);
    size_t pos = data.find(":");
    if (pos == std::string::npos)
    {
        size_t posX = data.find(" ");
        data = ":" + data.substr(0, posX);
    }
    std::string output = ":"+ user->GetNickname() + "!"+ user->GetRealName() +"@" + user->GetClientIP() + " PRIVMSG "+ name + " " + data + "\r\n";
    send(socketFailed, output.c_str(), output.length(), 0);
}


void Database::StartCommunication(int UserSocket, std::string data)
{
    std::string msg;
    STORE List;

    Database *service = Database::GetInstance();
    std::string username = service->GetUserBySocket(UserSocket);
    CleanInput(data, ' ');

    if (data.empty() || Protection(data))
    {
        ERR_NORECIPIENT_411(username, UserSocket);
        return;
    }

    List = GetSendingList(data, msg);
    if (msg.empty() || Protection(msg))
    {
        ERR_NOTEXTTOSEND_412(username, UserSocket);
        return ;
    }
    for(size_t i = 0; i < List.size(); i++)
    {
        if (List[i][0] == '#')
            service->DisplayMessages(msg, List[i], username, UserSocket);
        else
            service->PRIVMessages(msg, List[i], username);
    }
}

void Database::DisplayMessages(std::string data, std::string name, std::string username, int UserSocket)
{
    std::string output;
    bool is_out = undefine;
    std::stringstream Respond;
	Channel *channel = GetChannel(name);
	Client *user = GetClient(username);

	for (SYSTEM_CHANNEL::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->first == name) {
			is_out = 1;
			break;
		}
	}
    if (is_out == undefine)
    {
        ERR_NOSUCHCHANNEL_403(username, name, UserSocket);
        return ;
    }
	int state = channel->DoesClientExist(username);
	if (state == 0 || state == 3) {
		ERR_NOTONCHANNEL_442(username, UserSocket, name);
		return ;
	}
    size_t pos = data.find(":");
    if (pos == std::string::npos)
    {
        size_t posX = data.find(" ");
        data = ":" + data.substr(0, posX);
    }
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end() && is_out; ++it)
    {
        if (it->second->ChannelList(name))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine && socket != UserSocket)
            {
                it->second->GetNickname();
                output = ":" + username + "!" + user->GetRealName() + "@" + user->GetClientIP() + " PRIVMSG "+ name + " " + data + "\r\n";
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Database::AddChannel(const std::string& name, Channel* channel)
{
	channels[name] = channel;
}

void Database::PrintChannels()
{
	std::cout << "Channels:" << std::endl;
	for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it) 
		std::cout << "=> [" << it->first << "]  --> " << it->first.length() << std::endl;
}

std::string Database::GetUserBySocket(int UserSocket)
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->GetSocket() == UserSocket)
			return it->first;
	}
	return ("");
}

int Database::GetUserSocket(std::string name)
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first == name)
			return it->second->GetSocket();
	}
	return (undefine);
}

Channel* Database::GetChannel(const std::string& name)
{
	for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (name == it->first)
			return channels[name];
	}
	return undefine;
}

// *************************************************************************

void    Database::HandleTopic(std::string data, int UserSocket)
{
	Database *database = Database::GetInstance();
	std::string username = database->GetUserBySocket(UserSocket);
	std::string N_topic;
	std::string command;
	std::string C_N;
	bool    two_dots = false;
	std::stringstream  ss(data);

	ss >> command >> C_N;
	std::getline(ss, N_topic);

	std::string Channel_N(C_N);

	if (Channel_N[0] == '\0') {
		ERR_461_NEEDMOREPARAMS(username, command, UserSocket);
		return ;
	}
	Channel_N = ExtractChannelName(Channel_N);
	
	if (Channel_N.empty()) {
		ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
		return ;
	}
	std::map<std::string, Channel *>::iterator it = channels.find(Channel_N);
	if (it == channels.end())
	{
		ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
		return ;
	}
	N_topic = ExtractTopic(N_topic, &two_dots);
	if (N_topic.empty() && !two_dots) {
		if (it->second == undefine) {
			ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		else {
			if (it->second->getTopic().empty())
				RPL_NOTOPIC_331(username, Channel_N, UserSocket);
			else
				RPL_TOPIC_332(username, Channel_N, it->second->getTopic(), UserSocket);
		}
	}
	else if (N_topic.empty() && two_dots) {
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		else if (state == 1) {
			it->second->setTopic("");
			std::string broadcast = RPL_TOPIC(username, Channel_N, "");
			it->second->BroadCastMessage(broadcast);
		}
		else if (it->second->isProtectedTopic() && (state == 2 || state == 3)) {
			ERR_482_CHANOPRIVSNEEDED(username, Channel_N, UserSocket);
			return ;
		}
		else {
			it->second->setTopic("");
			std::string broadcast = RPL_TOPIC(username, Channel_N, "");
			it->second->BroadCastMessage(broadcast);
		}
	}
	else if (!N_topic.empty()) {
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		else if (state == 1) {
			it->second->setTopic(N_topic);
			std::string broadcast = RPL_TOPIC(username, Channel_N, N_topic);
			it->second->BroadCastMessage(broadcast);
		}
		else if (it->second->isProtectedTopic() && (state == 2 || state == 3)) {
			ERR_482_CHANOPRIVSNEEDED(username, Channel_N, UserSocket);
			return ;
		}
		else {
			it->second->setTopic(N_topic);
			std::string broadcast = RPL_TOPIC(username, Channel_N, N_topic);
			it->second->BroadCastMessage(broadcast);
		}
	}
}

void    Database::HandleMode(std::string data, int UserSocket)
{
	bool addMode = true;
	std::stringstream ss(data);

	std::string UserName = GetUserBySocket(UserSocket);
	std::string	command, C_N, Modes_, remain;

	ss >> command >> C_N >> Modes_;

	std::vector<std::string> m_args;

	while (ss >> remain)
		m_args.push_back(remain);

	std::string channelName(C_N);
	std::string modes(Modes_);
    
	if (modes.empty() || modes.back() != '\0')
        modes.push_back('\0');

	if (modes[0] == '\0') {
		modes = "";
	}

	if (channelName[0] == '\0') {
		ERR_461_NEEDMOREPARAMS(UserName, command, UserSocket);
		return ;
	}

	channelName = ExtractChannelName(channelName);
	if (channelName.empty()) {
		ERR_403_NOSUCHCHANNEL(UserName, channelName, UserSocket);
		return ;
	}

	int state = channels[channelName]->DoesClientExist(UserName);

	if (state == 0 || state == 3) {
		ERR_442_NOTONCHANNEL(UserName, channelName, UserSocket);
		return ;
	}
	else if (modes.empty()) {
		RPL_324_CHANNELMODEIS(UserName, channelName,  UserSocket);
		return ;
	}
	else if (state == 2) {
		ERR_482_CHANOPRIVSNEEDED(UserName, channelName, UserSocket);
		return ;
	}

	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
	for (size_t i = 0; i < modes.size() - 1; i++) {
		char c = modes[i];
		if (c == '+' || c == '-') {
			addMode = (c == '+');
		}
		else {
			applyModeChange(c, addMode, it->second, UserName, m_args);
		}
	}
}

void	Database::HandleInvite(std::string data, int UserSocket)
{
	int state;
	std::string command, username, C_N, target;

	std::stringstream ss(data);
	ss >> command >> C_N >> target;
	username = GetUserBySocket(UserSocket);

	std::string channelName(C_N);
	if (channelName[0] == '\0') {
		ERR_461_NEEDMOREPARAMS(username, command, UserSocket);
		return ;
	}

	channelName = ExtractChannelName(channelName);
	if (channelName.empty()) {
		ERR_403_NOSUCHCHANNEL(username, channelName, UserSocket);
		return ;
	}

	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	std::map<std::string, Channel *>::iterator iter = channels.find(channelName);
	state = iter->second->DoesClientExist(username);
	if (state == 0 || state == 3) {
		ERR_442_NOTONCHANNEL(username, channelName, UserSocket);
		return ;
	}
	else if ((state == 2) && iter->second->isInviteOnly()) {
		ERR_482_CHANOPRIVSNEEDED(username, channelName, UserSocket);
		return ;
	}
	else {
		Client *targetClient = GetClient(target);
		if (!targetClient) {
			ERR_NOSUCHNICK(username, target, UserSocket);
			return ;
		}
		state = it->second->DoesClientExist(target);
		if (state == 0 || state == 3) {
			RPL_341_INVITING(username, target, channelName, UserSocket);
			std::string notice = ":" + username + " INVITE " + target + " " + channelName + "\n";
			send(GetUserSocket(target), notice.c_str(), notice.length(), 0);
			return ;
		}
		else {
			ERR_443_USERONCHANNEL(username, target, channelName, UserSocket);
			return ;
		}
	}
}

void	Database::HandleBot(std::string data, int UserSocket) {
	std::string command, username, arg, message;
	std::stringstream ss(data);
	Client *user = GetClient(GetUserBySocket(UserSocket));

	ss >> command >> arg;
	username = GetUserBySocket(UserSocket);

	if (arg.empty() || arg[0] == '\0') {
		std::string reply = ":BOT : Hi, i'm your bot, i can help you explaining the commands\n"
		":BOT : You can use the following commands :\n"
		":BOT : BOT JOIN\n" ":BOT : BOT PART\n" ":BOT : BOT TOPIC\n" ":BOT : BOT MODE\n" ":BOT : BOT INVITE\n" ":BOT : BOT KICK\n" ":BOT : BOT PRIVMSG\n"
		":BOT : After using one of the noted commands, i can explain to you how to use it and what are they for\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}

	else if (arg == "JOIN" || arg == "join") {
		std::string reply = ":BOT : The JOIN command is used to join a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : JOIN #channel; where #channel is the name of the channel you want to join\n"
		":BOT : if you want to join multiple channels, you can use the following syntax :\n"
		":BOT : JOIN #channel1,#channel2,#channel3, ... ,#channelN\n"
		":BOT : If the channel does not exist, it will be created\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "PART" || arg == "part") {
		std::string reply = ":BOT : The PART command is used to leave a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : PART #channel; where #channel is the name of the channel you want to leave\n"
		":BOT : if you want to leave multiple channels, you can use the following syntax :\n"
		":BOT : PART #channel1,#channel2,#channel3, ... ,#channelN\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "TOPIC" || arg == "topic") {
		std::string reply = ":BOT : The TOPIC command is used to set the topic of a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : TOPIC #channel :new_topic; where #channel is the name of the channel and new_topic is the new topic\n"
		":BOT : if you want to get the topic of a channel, you can use the following syntax :\n"
		":BOT : TOPIC #channel\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "MODE" || arg == "mode") {
		std::string reply = ":BOT : The MODE command is used to change the mode of a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : MODE #channel +/-mode; where #channel is the name of the channel and mode is the mode you want to change\n"
		":BOT : The followig are the available modes :\n"
		":BOT : MODE #<channel> (+/-)i : invite only, setting this mode will remove or allow only invited users to join the <channel>\n"
		":BOT : (+/-)o <user> : operator, setting this mode will give or remove the <user> operator privileges\n"
		":BOT : (+/-)t : topic protection, setting this mode will prevent members from changing the topic of the <channel>\n"
		"Only the channel operator can change the mode of the channel\n"
		":BOT : (+/-)k <password> : password, setting this mode will require a password to join the <channel>\n"
		":BOT : (+/-)l <number> : limit, setting this mode will limit the number of users that can join the <channel>\n"
		":BOT : if you want to get the mode of a channel, you can use the following syntax :\n"
		":BOT : MODE #<channel>\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "INVITE" || arg == "invite") {
		std::string reply = ":BOT : The INVITE command is used to invite a user to a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : INVITE #channel username; where #channel is the name of the channel and username is the name of the user you want to invite\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "KICK" || arg == "kick") {
		std::string reply = ":BOT : The KICK command is used to kick a user from a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : KICK #channel username; where #channel is the name of the channel and username is the name of the user you want to kick\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else if (arg == "PRIVMSG" || arg == "privmsg") {
		std::string reply = ":BOT : The PRIVMSG command is used to send a private message to a user or a channel\n"
		":BOT : You can use the following syntax :\n"
		":BOT : PRIVMSG #channel :message; where #channel is the name of the channel and message is the message you want to send\n"
		":BOT : if you want to send a private message to a user, you can use the following syntax :\n"
		":BOT : PRIVMSG username :message\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
	else {
		std::string reply = ":BOT : The command you entered is not valid\n"
		":BOT : You can use the following commands :\n"
		":BOT : /BOT JOIN\n" ":BOT : /BOT PART\n" ":BOT : /BOT TOPIC\n" ":BOT : /BOT MODE\n" ":BOT : /BOT INVITE\n" ":BOT : /BOT KICK\n" ":BOT : /BOT PRIVMSG\n";
		std::string rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		while (1) {
			std::string _data = ":BOT!BOT@" + user->GetClientIP() + " PRIVMSG "+ username + " " + rpl + "\r\n";
			send(UserSocket, _data.c_str(), _data.length(), 0);
			reply = reply.substr(reply.find_first_of('\n') + 1);
			if (reply.empty())
				break;
			rpl = reply.substr(reply.find_first_not_of('\n'), reply.find_first_of('\n'));
		}
		return ;
	}
}


std::string Database::ExtractChannelName(std::string input) {
	size_t pos;

	if (input.empty() || input[0] != '#') {
		return "";
	}
	pos = input.find_first_of('#');
	if (pos == std::string::npos) {
		return "";
	}
	std::string Channel = input.substr(pos, input.length() - pos);

	if (!DoesChannelExist(Channel))
		return "";
	return Channel;
}

std::string Database::ExtractTopic(std::string input, bool *two_dots) {
	size_t pos;

	if (input.empty() || input[0] == '\0') {
		*two_dots = false;
		return "";
	}
	pos = input.find_first_not_of(' ');
	if (pos == std::string::npos) {
		*two_dots = false;
		return "";
	}
	else if (input[pos] == ':')
		pos++;
	else {
		*two_dots = false;
		std::stringstream ss(input);
		ss >> input;
		return input;
	}
	*two_dots = true;
	return input.substr(pos, input.length() - pos);
}

void	Database::applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName, std::vector<std::string> &m_args)
{
	std::string broadcast;
	int TargetSocket = 0;
	Client *User = GetClient(UserName);
	int UserSocket = GetUserSocket(UserName);
	switch (mode) {
		case 'i':
			channel->setInviteOnly(addMode);
			if (addMode)
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +i " + GetUserBySocket(TargetSocket) + "\r\n";
			else
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " -i " + GetUserBySocket(TargetSocket) + "\r\n";
			channel->BroadCastMessage(broadcast);
			break;
		case 't':
			channel->setProtectedTopic(addMode);
			if (addMode)
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +t " + GetUserBySocket(TargetSocket) + "\r\n";
			else
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " -t " + GetUserBySocket(TargetSocket) + "\r\n";
			channel->BroadCastMessage(broadcast);
			break;
		case 'l':
			if (channel->setUserLimit(m_args, UserName, addMode)) {
				if (addMode)
					broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +l " + GetUserBySocket(TargetSocket) + "\r\n";
				else
					broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " -l " + GetUserBySocket(TargetSocket) + "\r\n";
				channel->BroadCastMessage(broadcast);
			}
			break;
		case 'k':
			if (channel->setKey(m_args, addMode, UserName)) {
			if (addMode)
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +k " + GetUserBySocket(TargetSocket) + "\r\n";
			else
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " -k " + GetUserBySocket(TargetSocket) + "\r\n";
			channel->BroadCastMessage(broadcast);
			}
			break;
		case 'o':
			TargetSocket = GetUserSocket(m_args[0]);
			if (channel->SetOperator(UserName, addMode, m_args)) {
			if (addMode)
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +o " + GetUserBySocket(TargetSocket) + "\r\n";
			else
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " -o " + GetUserBySocket(TargetSocket) + "\r\n";
			channel->BroadCastMessage(broadcast);
			}
			break;
		case 's':
			break;
		case 'n':
			break;
		default:
			ERR_472_UNKNOWNMODE(UserName, mode, UserSocket);
			break;
	}
}

bool Database::DoesChannelExist(std::string channelName)
{
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	if (it != channels.end())
		return true;
	return false;
}

bool Database::IsUserInChannel(std::string channelName, std::string UserName)
{
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	if (it != channels.end())
	{
		if (it->second->DoesClientExist(UserName) == 1)
			return true;
	}
	return false;
}

// -----------------------------------------------------------

void Database::RemoveClient(int fd)
{
	std::string username = GetUserBySocket(fd);
	Client *client = GetClient(username);
	std::vector<std::string> channels = client->GetChannels();

	for (size_t i = 0; i < channels.size(); i++) {
		Channel *channel = GetChannel(channels[i]);
		channel->PartFromChannels(username);
		NoticeUserPART(channels[i], username, client->GetSocket(), client->GetClientIP(), "");
	}
	clients.erase(username);
}

Database::~Database()
{
	std::cout << "Database destructor" << std::endl;
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it) {
		delete it->second;
		clients.erase(it);
	}
	for (SYSTEM_CHANNEL::iterator it = channels.begin(); it != channels.end(); ++it) {
		delete it->second;
		channels.erase(it);
	}
}
