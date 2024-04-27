#include "Database.hpp"
#include "Client.hpp"
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

void Database::AddClient(Client *client)
{
	clients.insert(std::make_pair(client->GetNickname(), client));
	std::cout << "size ==> " << clients.size() << std::endl;
}

Client* Database::GetClient(const std::string& name)
{
	return clients[name];
}

void Database::SetServerIP(struct in_addr host)
{
	this->server_ip = host;
}

string Database::GetServerIP(void)
{
	string HOST(inet_ntoa(server_ip));
	return (HOST);
}

void Database::NoticeUserPART(string ChannelName, string username, int UserSocket, string IP, string msg)
{
    string output;

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
        output = ":" + username + "!" + user->GetNickname() +  "@" + IP + " PART " + ChannelName + " " + msg + "\n";

    }
    else
        output = ":" + username + "!" + user->GetNickname() +  "@" + IP + " PART " + ChannelName + "\n";
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

void Database::NoticeUserKICK(string ChannelName, string username, string IP, string target, string msg)
{
    string output;

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

void Database::NoticeUserHasJoined(string name, string username, int UserSocket, string IP)
{
	(void)IP;
	std::cout << "RAK HNA AZABI" << std::endl;
	string output;
	std::stringstream Respond;
	// std::string _username = username.substr(0, username.size() - 1);
	Client *User = GetClient(username);
	if (!User) {
		std::cout << "User not found" << std::endl;
		return ;
	}

	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->ChannelList(name))
		{
			int socket = it->second->GetSocket();
			if (socket > undefine)
			{
				Respond.str("");
				std::cout << "Channel ==> [" << name << "] size ==> " << name.size() << std::endl;
				// std::string channel_N = name.substr(0, name.size() - 1);
				// Respond << ":test!test@127.0.0.1 JOIN #chan\r\n";
				std::cout << "USER IP ==>> " << User->GetClientIP() << std::endl;
				Respond << ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " JOIN " + name << "\r\n";
				output = Respond.str();
				send(socket, output.c_str(), output.length(), 0);
			}
		}
	}
	Channel *channel = GetChannel(name);
	if (channel == undefine)
		return ;
	std::cout << "-- Channel Name : " << name << std::endl;
	// if (channel->getTopic().empty())
	// 	RPL_NOTOPIC_331(username, name, UserSocket);
	// else
	// 	RPL_TOPIC_332(username, name, channel->getTopic(), UserSocket);
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
	Respond << ":irc.1337.com 366 " + username << " " + name + " :End of /NAMES list." << std::endl;
	// Respond << ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + getIPAddress() + " :End of /NAMES list." << "\r\n";
	output = Respond.str();
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::NoticeUserLogout(string name, string username)
{
	string output;
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

void Protection475(string name, string username, int UserSocket, string IP)
{
	string output = ":irc.1337.com" + IP + " 475 " + username + " " + name + " :Cannot join channel (+k) - bad key\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 475 adrian #test :Cannot join channel (+k) - bad key
*/

void Protection471(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 471 " + username + " " + name + " :Cannot join channel (+l)\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 471 alice #test :Cannot join channel (+l)
*/

void Protection474(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 474 " + username + " " + name + " :Cannot join channel (+b)\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 474 alice #test :Cannot join channel (+b)
*/

void Protection473(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 473 " + username + " " + name + " :Cannot join channel (+i)\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void xProtection403(string command, int UserSocket, string username)
{
	string output = ":irc.1337.com 403 " + username + " " + command + " :Unknown command\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*

void Database::OutFromAllChannels(int UserSocket)
{
	Database *service = Database::GetInstance();
	string username = service->GetUserBySocket(UserSocket);
	Client *user = service->GetClient(username);

	STORE UserChannels = user->GetAllChannels();
}
*/


void Database::HandelMultiChannel(string data, int UserSocket)
{
    string args;
    SYSTEM_KEYVAL::iterator it;

    CleanInput(data, ' ');
    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
	std::cout << "username : " << username << " length ==> " << username.length() << std::endl;
	// if (username.back() != '\0') {
	// 	username.push_back('\0');
	// 	std::cout << "username : " << username << " length ==> " << username.length() << std::endl;
	// }
    Client *user = service->GetClient(username);
	std::cout << "user : " << user <<  std::endl;
	if (!user) {
		std::cout << "User not found" << std::endl;
		return ;
	}
	std::cout << "[1]" <<  std::endl;
    if (Protection(data) || data.empty())
    {
        ERR_NEEDMOREPARAMS_461("JOIN",UserSocket, username);
        return ;
    }
	std::cout << "[2]" <<  std::endl;
    string IP = user->GetClientIP();
	std::cout << "ip mn client : " << IP << std::endl;
	std::cout << "[3]" <<  std::endl;
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
	std::cout << "[4]" <<  std::endl;
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
		std::cout << "[5]" <<  std::endl;
        std::string EXIST(it->first);
		std::cout << "EXIST ==> " << EXIST << " length ==> " << EXIST.length() << std::endl;
		// EXIST = ExtractChannelName(EXIST);
		// std::cout << "EXIST ==> " << EXIST << " length ==> " << EXIST.length() << std::endl;
    
        // if (EXIST.empty() || EXIST.back() != '\0')
        //     EXIST.push_back('\0');
        
		// std::cout << "EXIST ==> " << EXIST << " length ==> " << EXIST.length() << std::endl;
        if (user->LimitedChannels())
        {
            ERR_TOOMANYCHANNELS_405(EXIST, UserSocket, username);
            break;
        }
		std::cout << "[6]" <<  std::endl;
        if (user->ChannelList(EXIST))
            continue;
        Channel *channel = service->GetChannel(EXIST);
        if (channel != undefine)
        {
			std::cout << "[7]" <<  std::endl;
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
			std::cout << "[8]" <<  std::endl;
            if (channel->GetLimit() != -1)
            {
                bool OutOfBound = channel->CountMembers() + channel->CountAdmins() + 1 > channel->GetLimit();
                if (OutOfBound)
                {
                    ERR_CHANNELISFULL_471(EXIST, UserSocket, username, IP);
                    continue;
                }
            }
			std::cout << "[9]" <<  std::endl;
            if (channel->UserIsBanned(username))
            {
                ERR_BANNEDFROMCHAN_474(EXIST, UserSocket, username, IP);
                continue;
            }
			std::cout << "[10]" <<  std::endl;
			std::cout << "OUT" << std::endl;
        }
        if (channel == undefine)
        {
			std::cout << "first creation" << std::endl;
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

void Database::HandelMultiPART(string data, int UserSocket)
{
    string args;
    SYSTEM_KEYVAL::iterator it;

    CleanInput(data, ' ');
    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
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
        string NotFound = part1.substr(0, posX);
        ERR_NOSUCHCHANNEL_403(username, NotFound, UserSocket);
        return ;
    }
    std::getline(Object, part2);
    bool is_inChannel = undefine;
    string IP = user->GetClientIP();
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
        string EXIST(it->first);

        // if (EXIST.empty() || EXIST.back() != '\0')
        //     EXIST.push_back('\0');

		SYSTEM_CHANNEL::iterator xit;
        for (xit = service->channels.begin(); xit != service->channels.end() && !is_inChannel; ++xit)
        {
            string Detected(xit->first);

            // if (Detected.empty() || Detected.back() != '\0')
            //     Detected.push_back('\0');
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

void Database::HandelKick(string data, int UserSocket)
{
    CleanInput(data, ' ');

    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
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

    string ChannelName(part1);
    string TrgetUsername(part2);

    // if (ChannelName.empty() || ChannelName.back() != '\0')
    //     ChannelName.push_back('\0');
    // if (TrgetUsername.empty() || TrgetUsername.back() != '\0')
    //     TrgetUsername.push_back('\0');

    bool not_found = undefine;
    string IP = user->GetClientIP();
    SYSTEM_CHANNEL::iterator xit;
    for (xit = service->channels.begin(); xit != service->channels.end(); ++xit)
    {
        string Detected(xit->first);

        // if (Detected.empty() || Detected.back() != '\0')
        //     Detected.push_back('\0');
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
/*
bool Channel::UserIsBanned(std::string username)

void Channel::BanMember(std::string username)
421) 
  "<client> <command> :Unknown command"

*/

void Protection421(string command, int UserSocket, string username)
{
	string output = ":irc.1337.com 421 " + username + " " + command + " :Unknown command\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Protection403(string command, int UserSocket, string username)
{
	string output = ":irc.1337.com 403 " + username + " " + command + " :Unknown command\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::ParseUserInput(string data, int UserSocket)
{
	string args;
	string command;

	GetCommand(data.c_str(), command, args);

	Database *service = Database::GetInstance();
	string username = service->GetUserBySocket(UserSocket);
	//Client *user = service->GetClient(username);


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
	else if (command == "/BOT" || command == "/bot") {
		service->HandleBot(data, UserSocket);
		return ;
	}

	CleanInput(data, ' ');
    std::stringstream cmd(data);
    cmd >> command;
    std::getline(cmd, args);
	
	std::cout << "command : " << command << " length ==> " << command.length() << std::endl;

	if (command == "JOIN" || command == "join")
		HandelMultiChannel(args, UserSocket);
	else if (command == "PRIVMSG" || command == "privmsg")
		service->StartCommunication(UserSocket, args);
	else if (command == "PART" || command == "part")
        return service->HandelMultiPART(args, UserSocket);
	else if (command == "KICK" || command == "kick")
        service->HandelKick(args, UserSocket);
	else
		Protection421(command, UserSocket, username);
	PrintChannels();
}

void ERR_NOSUCHNICK(string username, string target, int UserSocket)
{
	string output = ":irc.1337.com 401 " + username + " " + target + " :No such nick/channel\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::PRIVMessages(string data, string name, string username)
{
    int senderSocket = undefine;
    int socketFailed = GetUserSocket(name);

	std::cout << "***************> name : " << name << std::endl;
	std::cout << "***************> Socket number : " << socketFailed << std::endl;
    string USER = "";
    string hostname = "";

    // if (username.empty() || username.back() != '\0')
    //     username.push_back('\0');
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
		std::cout << "=====> it->first : " << it->first << std::endl;
		std::cout << "=====> it->second->GetNickname() : " << it->second->GetNickname() << std::endl;
        string CHECK(it->first);
        // if (CHECK.empty() || CHECK.back() != '\0')
        //     CHECK.push_back('\0');
		std::cout << "______________________CHECK == [" << CHECK << "] length ==> " << CHECK.length() << std::endl;
		std::cout << "______________________username == [" << username << "] length ==> " << username.length() << std::endl;
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
	// std::string _username = username.substr(0, username.size() - 1);
    Client *user = GetClient(username);
	// _username = name.substr(0, name.size() - 1);
	// if (!user) {
	// 	std::cout << "User not found" << std::endl;
	// 	return ;
	// }
    size_t pos = data.find(":");
    if (pos == std::string::npos)
    {
        size_t posX = data.find(" ");
        data = ":" + data.substr(0, posX);
    }
		//    std::cout << "--------------- HNA FIN TRA L7WA -------------------------- " << std::endl;
    std::string output = ":"+ user->GetNickname() + "!"+ user->GetUsername() +"@" + user->GetClientIP() + " PRIVMSG "+ name + " " + data + "\r\n";
	std::cout << "output : " << output << std::endl;
		//    std::cout << "--------------- SFI RAH 5REJ -------------------------- " << std::endl;
    // string output = ":"+ username + "!" + USER + "@" + hostname + " PRIVMSG " + name + " " + data + "\n";
    send(socketFailed, output.c_str(), output.length(), 0);
}


void Database::StartCommunication(int UserSocket, string data)
{
    string msg;
    STORE List;

	std::cout << "===> UserSocket : " << UserSocket << std::endl;
    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    CleanInput(data, ' ');

    if (data.empty() || Protection(data))
    {
        ERR_NORECIPIENT_411(username, UserSocket);
        return;
    }

    List = GetSendingList(data, msg);
	// List[1] = List[1].substr(0, list[1].size() - 1);
    if (msg.empty() || Protection(msg))
    {
        ERR_NOTEXTTOSEND_412(username, UserSocket);
        return ;
    }
    for(size_t i = 0; i < List.size(); i++)
    {
        // if (List[i].empty() || List[i].back() != '\0')
        //     List[i].push_back('\0');
        if (List[i][0] == '#')
            service->DisplayMessages(msg, List[i], username, UserSocket);
        else
            service->PRIVMessages(msg, List[i], username);
    }
}

void Database::DisplayMessages(string data, string name, string username, int UserSocket)
{
    string output;
    bool is_out = undefine;
    std::stringstream Respond;
    // int socketFailed = undefine;
	Channel *channel = GetChannel(name);
	Client *user = GetClient(username);

    // if (name.empty() || name.back() != '\0')
	// {
	// 	std::cout << YELLOW "RA D5EL WLD L9A7BA" RESET << std::endl;
    //     name.push_back('\0');
	// }
	// name = ExtractChannelName(name);
    // if (username.empty() || username.back() != '\0')
    //     username.push_back('\0');
    // for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    // {
    //     string CHECK(it->first);
    //     // if (CHECK.empty() || CHECK.back() != '\0')
    //     //     CHECK.push_back('\0');
	// 	std::cout << YELLOW "CHECK == [" << CHECK << "] length ==> " << CHECK.length() << std::endl;
	// 	std::cout << "username == [" << username << "] length ==> " << username.length() << RESET << std::endl;
    //     if (it->second->ChannelList(name) && CHECK == username)
    //         is_out = 1;
    //     if (CHECK == username)
    //         socketFailed = it->second->GetSocket();
    // }
	for (SYSTEM_CHANNEL::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->first == name) {
			is_out = 1;
			break;
		}
	}
    if (is_out == undefine)
    {
		std::cout << YELLOW "RA D5EL WLD L9A7BA [22]" RESET << std::endl;
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
				// std::string Channel_N = name.substr(0, name.size() - 1);
                output = ":"+username+ "!"+it->second->GetUsername()+"@" + user->GetClientIP() + " PRIVMSG "+ name + " " + data + "\r\n";
                // output = ":"+username+ "!"+it->second->GetNickname()+"@" + it->second->GetClientIP() + " PRIVMSG "+name+" "+data+"\n";
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Database::AddChannel(const std::string& name, Channel* channel)
{
	channels[name] = channel;
	//PrintChannels();
}

void Database::PrintChannels() 
{
	std::cout << "Channels:" << std::endl;
	for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it) 
		std::cout << "=> [" << it->first << "]  --> " << it->first.length() << std::endl;
}

string Database::GetUserBySocket(int UserSocket)
{
	std::cout << "====> UserSocket : " << UserSocket << std::endl;
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		std::cout << "====> it->second->GetSocket() : " << it->second->GetSocket() << std::endl;
		if (it->second->GetSocket() == UserSocket)
			return it->first;
	}
	return ("");
}

int Database::GetUserSocket(string name)
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		std::cout << "/////////////////====> it->first : [" << it->first << "]" << "length == " << it->first.length() << std::endl;
		std::cout << "/////////////////====> name : [" << name << "]" << "length == " << name.length() << std::endl;
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

	string Channel_N(C_N);
    
    // if (Channel_N.empty() || Channel_N.back() != '\0')
    //     Channel_N.push_back('\0');

	std::cout << "command : " << command << std::endl;
	std::cout << "C_N : " << C_N << std::endl;
	std::cout << "N_topic : " << N_topic << std::endl;
	std::cout << "username : " << username << std::endl;

	// condition 0 : "TOPIC" --> need more params
	if (Channel_N[0] == '\0') {
		ERR_461_NEEDMOREPARAMS(username, command, UserSocket);
		return ;
	}
	std::cout << "--> [1] channel_N : [" << C_N << "]" << "length = " << C_N.length() << std::endl;
	Channel_N = ExtractChannelName(Channel_N);
	
	if (Channel_N.empty()) {
		ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
		return ;
	}
	std::cout << "--> [2] Channel_N : [" << Channel_N << "]" << "length = " << Channel_N.length() << std::endl;
	std::map<std::string, Channel *>::iterator it = channels.find(Channel_N);
	if (it == channels.end())
	{
		ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
		return ;
	}
	else {
		std::cout << "Channel found" << std::endl;
	}
	N_topic = ExtractTopic(N_topic, &two_dots);
	std::cout << "N_topic extracted : [" << N_topic << "]" << std::endl;
	// condition 1 : "TOPIC #channel" -> get the topic
	if (N_topic.empty() && !two_dots) {
		// --- if topic is empty then the client is asking for the topic ---
    	std::cout << "IN DoesClientExist" << std::endl;
		if (it->second == undefine) {
			std::cout << "Channel not found" << std::endl;
			ERR_403_NOSUCHCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		int state = it->second->DoesClientExist(username);
    	std::cout << "OUT DoesClientExist" << std::endl;
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, Channel_N, UserSocket);
			return ;
		}
		else {
			std::cout << "----------- [2] -----------" << std::endl;
			if (it->second->getTopic().empty())
				RPL_NOTOPIC_331(username, Channel_N, UserSocket);
			else
				RPL_TOPIC_332(username, Channel_N, it->second->getTopic(), UserSocket);
			std::cout << "----------- [3] -----------" << std::endl;
			
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
	// condition 2 : "TOPIC #channel :" -> set the topic to empty
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

	std::cout << "command : " << command << std::endl;
	std::cout << "C_N : " << C_N << std::endl;
	std::cout << "Modes_ : " << Modes_ << std::endl;

	std::vector<std::string> m_args;

	while (ss >> remain)
		m_args.push_back(remain);

	string channelName(C_N);
	string modes(Modes_);
    
	if (modes.empty() || modes.back() != '\0')
        modes.push_back('\0');

	if (modes[0] == '\0') {
		modes = "";
	}

    // if (channelName.empty() || channelName.back() != '\0')
    //     channelName.push_back('\0');

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

	std::cout << "command : " << command << std::endl;
	std::cout << "username : " << username << std::endl;
	std::cout << "C_N : " << C_N << std::endl;

	std::string channelName(C_N);

	std::cout << "[1] channelName : " << channelName << " length" << channelName.length() << std::endl;
	
	// if (channelName.empty() || channelName.back() != '\0')
	// 	channelName.push_back('\0');

	if (channelName[0] == '\0') {
		ERR_461_NEEDMOREPARAMS(username, command, UserSocket);
		return ;
	}

	channelName = ExtractChannelName(channelName);

	std::cout << "[2] channelName : " << channelName << " length" << channelName.length() << std::endl;
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
		state = it->second->DoesClientExist(target);
		if (state == 0 || state == 3) {
			RPL_341_INVITING(username, target, channelName, UserSocket);
			std::string notice = ":" + username + " INVITE " + target + " " + channelName + "\n";
			send(GetUserSocket(target), notice.c_str(), notice.length(), 0);
			std::cout << "The target has been noticed" << std::endl;
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

	std::cout << "command : " << command << std::endl;
	std::cout << "username : " << username << std::endl;
	std::cout << "arg : " << arg << std::endl;

	if (arg.empty() || arg[0] == '\0') {
		std::string reply = ":BOT : Hi, i'm your bot, i can help you explaining the commands\n"
		":BOT : You can use the following commands :\n"
		":BOT : /BOT JOIN\n" ":BOT : /BOT PART\n" ":BOT : /BOT TOPIC\n" ":BOT : /BOT MODE\n" ":BOT : /BOT INVITE\n" ":BOT : /BOT KICK\n" ":BOT : /BOT PRIVMSG\n"
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

	std::cout << "Input is : [" << input << "]" << std::endl;
	if (input.empty() || input[0] != '#') {
		return "";
	}
	pos = input.find_first_of('#');
	if (pos == std::string::npos) {
		return "";
	}
	std::string Channel = input.substr(pos, input.length() - pos);

	std::cout << YELLOW "Extracted Channel is : [" << Channel << "]" << RESET << std::endl;
	if (!DoesChannelExist(Channel))
	{
		std::cout << "--- FALSE ---" << std::endl;
		return "";
	}
	std::cout << "Channel Extracted is : [" << Channel << "] length ==>" << Channel.length() << std::endl;
	return Channel;
}

std::string Database::ExtractTopic(std::string input, bool *two_dots) {
	size_t pos;
	size_t x = 0;

	if (!input.empty()) {
		x = input.find(13);
		std::cout << "x : " << x << std::endl;
		if (x != std::string::npos)
			input[x] = '\0';
	}
	if (input[0] == '\0') {
		std::cout << "empty input" << std::endl;
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
	std::cout << "applyModeChange : --> " << mode << std::endl;
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
			// ":" + _nickname + "!" + _username + "@" + getServerIP() + " " + "MODE " + channelName + " +o " + modeArg.front() + "\r\n";
			if (channel->SetOperator(UserName, addMode, m_args)) {
			if (addMode)
			{
				std::cout << "______________Target Name : " << GetUserBySocket(TargetSocket) << std::endl;
				std::cout << "______________IP : " << User->GetClientIP() << std::endl;
				broadcast = ":" + User->GetNickname() + "!" + User->GetUsername() + "@" + User->GetClientIP() + " MODE " + channel->ChannelName() + " +o " + GetUserBySocket(TargetSocket) + "\r\n";
			}
				// broadcast = ":324 " + UserName + " " + channel->ChannelName() + " +o\n";
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
	std::cout << "Channel Name in DCEX : [" << channelName << "] --> " << channelName.length() << std::endl;
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

// --------- ERRORS AND REPLYS ------------

void    Database::ERR_442_NOTONCHANNEL(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_NOTONCHANNEL(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_461_NEEDMOREPARAMS(std::string username, std::string command, int UserSocket)
{
	std::string error = ERR_NEEDMOREPARAMS(username, command);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_403_NOSUCHCHANNEL(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_NOSUCHCHANNEL(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_482_CHANOPRIVSNEEDED(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_CHANOPRIVSNEEDED(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::RPL_NOTOPIC_331(std::string username, std::string channelName, int UserSocket)
{
	std::string error = RPL_NOTOPIC(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}
void    Database::RPL_TOPIC_332(std::string username, std::string channelName, std::string topic, int UserSocket)
{
	std::string error = RPL_TOPIC(username, channelName, topic);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::RPL_324_CHANNELMODEIS(std::string username, std::string channelName, int UserSocket)
{
	std::cout << YELLOW "RPL_324_CHANNELMODEIS" << std::endl;
	std::cout << "channelName : [" << channelName << "]" RESET << std::endl;
	std::string modes = channels[channelName]->GetModes();
	std::string error = RPL_CHANNELMODEIS(username, channelName, modes);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_472_UNKNOWNMODE(std::string username, char mode, int UserSocket)
{
	std::string error = ERR_UNKNOWNMODE(username, mode);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_401_NOSUCHNICK(std::string username, std::string target, int UserSocket)
{
	std::string error = ERR_NOSUCHNICK_(username, target);
	send(UserSocket, error.c_str(), error.length(), 0);
}


void	Database::ERR_441_USERNOTINCHANNEL(std::string UserName, std::string target, std::string channelName, int UserSocket)
{
	std::string error = ERR_USERNOTINCHANNEL(UserName, target, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::RPL_341_INVITING(std::string UserName, std::string target, std::string channel, int UserSocket)
{
	Channel *channel_ = channels[channel];
	channel_->AddInvited(target);
	std::string error = RPL_INVITING(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_443_USERONCHANNEL(std::string UserName, std::string target, std::string channel, int UserSocket)
{
	std::string error = ERR_USERONCHANNEL(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}

std::string Database::getIPAddress() {
    char hostName[255];
    char ipAddress[255];
    
    // Get the hostname of the current machine
    if (gethostname(hostName, sizeof(hostName)) == -1) {
        perror("Error getting hostname");
        return "";
    }

    // Get host information using hostname
    struct hostent* hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL) {
        perror("Error getting host information");
        return "";
    }

    // Convert the first IP address to a string
    if (inet_ntop(AF_INET, hostInfo->h_addr_list[0], ipAddress, sizeof(ipAddress)) == NULL) {
        perror("Error converting IP address to string");
        return "";
    }

    return std::string(ipAddress);
}

// -----------------------------------------------------------

Database::~Database()
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
		delete it->second;
	for (SYSTEM_CHANNEL::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
}
