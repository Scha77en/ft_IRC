#include "Database.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>

Database* Database::DB = undefine;

Database	*Database::GetInstance()
{
	if (DB == undefine)
		DB = new Database();
	return DB;
}

void Database::AddClient(const std::string& name)
{
    Client* client = new Client();
	clients[name] = client;
}

Client* Database::GetClient(const std::string& name)
{
	return clients[name];
}

void GetCommand(const char* buffer, std::string& command, std::string& channel) 
{
    std::stringstream ss(buffer);
    ss >> command >> channel;
}

void RemoveNewLine(string &str)
{
    size_t pos = 0;
    while ((pos = str.find('\n', pos)) != std::string::npos)
        str.erase(pos, 1);
}

void Database::DisplayMessages(string data, string name, string username)
{
    string output;
    bool is_out = undefine;
    std::stringstream Respond;
    int socketFailed = undefine;

    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->ChannelList(name) && it->first == username)
            is_out = 1;
        if (it->first == username)
            socketFailed = it->second->GetSocket();
    }
    if (is_out == undefine)
    {

        Respond.str("");
        Respond << RED << "@" + username << BLUE << " Not In [#" << name << "]" << RESET << std::endl;
        output = Respond.str();
        send(socketFailed, output.c_str(), output.length(), 0);
    }
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end() && is_out; ++it)
    {
        if (it->second->ChannelList(name))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine)
            {
                Respond.str("");
                Respond << BLUE << "@" + username << GREEN << " [" << name << "] :" << RESET << data << std::endl;
                output = Respond.str();
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Database::NoticeUserHasJoined(string name, string username, int UserSocket)
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
                Respond << BLUE << "@" + username << RED << " Joined to (#" << name << ") " << RESET << std::endl;
                output = Respond.str();
                send(socket, output.c_str(), output.length(), 0);

            }
        }
    }
    SYSTEM_CHANNEL::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->first == name)
        {
            it->second->UsersInChannel(UserSocket, username);
            break;
        }
    }
    Respond.str("");
    Respond << BLUE << "(366) " + username << " #" + name << RED << " :End of /NAMES list " << RESET << std::endl;
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
                Respond << BLUE << "@" + username << RED << " is Logout From (#" << name << ") " << RESET << std::endl;
                output = Respond.str();
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void ExtractTwoRnages(string data, string &ch, string &ky) 
{
    std::stringstream Process(data);

    Process >> ch >> ky;
}

STORE TokenRangeExtract(string data)
{
    STORE Extracted;

    string MiniToken;
    std::istringstream Object(data);

    while (std::getline(Object, MiniToken, ','))
        Extracted.push_back(MiniToken);
    return (Extracted);
}

void Error403(int SK, string username, string channel)
{
    string output;
    std::stringstream Respond;

    Respond.str("");
    Respond << "(403)" << BLUE << " @" + username <<  " " + channel << RED << " :No such channel " << RESET << std::endl;
    output = Respond.str();
    send(SK, output.c_str(), output.length(), 0);
}

SYSTEM_KEYVAL parseChannels(string &input, int SK, string username) 
{
    SYSTEM_KEYVAL result;
    string ch, ky;
    STORE RangeChannels, RangeKeys;

    ExtractTwoRnages(input, ch, ky);

    RangeChannels = TokenRangeExtract(ch);
    RangeKeys = TokenRangeExtract(ky);

    for (size_t i = 0; i < RangeChannels.size(); ++i) 
    {
        if (i < RangeKeys.size())
        {
            if (RangeChannels[i][0] == '#')
            {
                RangeChannels[i].erase(RangeChannels[i].begin());
                result.push_back(std::make_pair(RangeChannels[i], RangeKeys[i]));
            }
            else
            {
                Error403(SK, username, RangeChannels[i]);
                return (result);
            }
        }
        else
        {
            if (RangeChannels[i][0] == '#')
            {
                RangeChannels[i].erase(RangeChannels[i].begin());
                result.push_back(std::make_pair(RangeChannels[i], ""));
            }
            else
            {
                Error403(SK, username, RangeChannels[i]);
                return (result);
            }
        }
    }
    return result;
}

bool Protection(string data)
{
    int count = 0, i = 0;

    while (data[i])
    {
        if (data[i] == ' ')
            count++;
        i++;
    }
    return (count == i);
}

bool Potection403(string data)
{
    size_t position = data.find('#');
    return (position == std::string::npos);
}

void Database::HandelMultiChannel(string data, int UserSocket)
{
    string args;
    string output;
    std::stringstream Respond;
    SYSTEM_KEYVAL::iterator it;

    Database *service = Database::GetInstance();

    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    size_t colonPos = data.find(' ');

    if (colonPos != std::string::npos)
        args = data.substr(colonPos + 1);
    if (Protection(args) || args.empty())
    {
        Respond.str("");
        Respond << "(461)" << BLUE << " @" + username << " JOIN :" << RED << "Not enough parameters " << RESET << std::endl;
        output = Respond.str();
        send(UserSocket, output.c_str(), output.length(), 0);
        return ;
    }
    if (Potection403(args))
    {
        Error403(UserSocket, username, args);
        return ;
    }
    // else send error msg in client socket

    SYSTEM_KEYVAL channels = parseChannels(args, UserSocket, username);
    for (it = channels.begin(); it != channels.end();it++)
    {
        Channel *channel = service->GetChannel(it->first);
        if (channel != undefine)
        {
            if (channel->GetSecretKey() != it->second)
            {
                Respond.str("");
                Respond << BLUE << it->first << RED << " is Protected ! " << RESET << std::endl;
                output = Respond.str();
                send(UserSocket, output.c_str(), output.length(), 0);
                continue;
            }
        }
        if (channel == undefine)
        {
            channel = new Channel(it->first, it->second);
            service->AddChannel(it->first, channel);
        }
        user->SaveChannel(it->first);
        user->ActiveInChannel(it->first);
        if (channel->FirstCreation())
            channel->addAdmin(username);
        else
            channel->addMember(username);
        NoticeUserHasJoined(it->first, username, UserSocket);
    }
}

void Protection421(string command, int UserSocket, string username)
{
    string output;
    std::stringstream Respond;

    Respond << "(421)" << BLUE << " @" + username <<  " " + command << RED << " :Unknown command " << RESET << std::endl;
    output = Respond.str();
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

    if (command == "JOIN" || command == "join")
        HandelMultiChannel(data, UserSocket);
    else if (command == "PRIVMSG" || command == "privmsg")
        service->StartCommunication(UserSocket, data.substr(8));
    else if (command == "TOPIC" || command == "topic")
        service->HandleTopic(data, UserSocket);
    else if (command == "MODE" || command == "mode") 
        service->HandleMode(data, UserSocket);
    else
        Protection421(command, UserSocket, username);
}

STORE GetSendingList(string data, string &message)
{
    std::stringstream Obj(data);
    
    std::string Ranges;
    std::string token;
    
    STORE List;
    
    int i = 0;
    while (std::getline(Obj, token, ':') && i < 2)
    {
        if (!i)
            Ranges = token;
        if (i)
            message = token;
        i++;
    }
    std::stringstream Object(Ranges);
    while (std::getline(Object, token, ','))
    {
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        List.push_back(token);
    }
    return (List);
}

void Database::PRIVMessages(string data, string name, string username)
{
    string output;
    std::stringstream Respond;

    int socketFailed = GetUserSocket(name);

    
    if (socketFailed == undefine)
    {

        Respond.str("");
        Respond << RED << "@" + name << BLUE << " Not Found !" << RESET << std::endl;
        output = Respond.str();
        send(socketFailed, output.c_str(), output.length(), 0);
        return ;
    }
    Respond.str("");
    Respond << BLUE << "@" + username << GREEN << " :" << RESET << data << std::endl;
    output = Respond.str();
    send(socketFailed, output.c_str(), output.length(), 0);
}

void Database::StartCommunication(int UserSocket, string data)
{
    string msg;
    STORE List;

    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    //Client *user = service->GetClient(username);
    //string name = user->GetChannelName(user->GetChannelID());
    List = GetSendingList(data, msg);
    for(size_t i = 0; i < List.size(); i++)
    {
        if (List[i][0] == '#')
            service->DisplayMessages(msg, List[i].substr(1), username);
        else
            service->PRIVMessages(msg, List[i], username);
    }
    
    /*
    size_t colonPos = data.find(' ');
    name = data.substr(0, colonPos);
    if (colonPos != std::string::npos)
        msg = data.substr(colonPos + 1);
    if (name == "QUIT")
    {
        NoticeUserLogout(msg, username);
        user->ChannelLogout(msg);
    }
    else
        service->DisplayMessages(msg, name, username);
    */
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
        std::cout << "=> " << it->first << std::endl;
}

string Database::GetUserBySocket(int UserSocket)
{
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->GetSocket() == UserSocket)
            return it->first;
    }
    return (undefine);
}

int Database::GetUserSocket(string name)
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

// **************************************************************************

void	Database::applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName)
{
    std::cout << "applyModeChange : --> " << mode << std::endl;
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
			channel->SetOperator(UserName, addMode);
			break;
		default:
			break;
	}
}

void    Database::HandleTopic(std::string data, int UserSocket)
{
	Database *service = Database::GetInstance();
	std::string username = service->GetUserBySocket(UserSocket);
    std::string N_topic;
    bool    two_dots;

    std::stringstream  ss(data);
	std::string channelName = ss.str().substr(6, ss.str().find(' ', 6) - 6);
    std::string channel_N = ExtractChannelName(channelName);
    if (channel_N.empty()) {
        std::string error = "(403) [" + username + "] " "[" + channelName + "] :No such channel\n";
        std::cout << RED << std::endl;
        send(UserSocket, error.c_str(), error.length(), 0);
        std::cout << GREEN << std::endl;
        return ;
    }
	std::string topic = ss.str().substr(ss.str().find(' ', 6) + 1);
    std::cout << "topic : [" << topic << "]" << std::endl;
    if (!topic.empty()) {
        N_topic = ExtractTopic(topic, &two_dots);
        if (N_topic.empty() && !two_dots) {
            std::string error = "(461) [" + username + "] " "[" + data + "] :Not enough parameters\n";
            std::cout << RED << std::endl;
            send(UserSocket, error.c_str(), error.length(), 0);
            std::cout << RESET << std::endl;
            return ;
        }
    }
    else {
        two_dots = false;
        N_topic = "";
    }
    std::cout << "channelName : [" << channel_N << "] N_topic : [" << N_topic << "]" << std::endl;
	std::map<std::string, Channel *>::iterator it = channels.find(channel_N);
	if (it != channels.end())
	{
        std::cout << "GOT INTO TOPIC" << std::endl;
        int v = it->second->DoesClientExist(username);
        std::cout << "v = " << v << std::endl;
		if (v != 0 && !N_topic.empty() && it->second->isProtectedTopic()) {
            std::cout << "[1]" << std::endl;
            if (v == 1) {
            std::cout << "[2]" << std::endl;
                it->second->setTopic(N_topic);
                std::string broadcast = "(332) TOPIC " + channel_N + " :" + N_topic + "\n";
                // send(UserSocket, broadcast.c_str(), broadcast.length(), 0);
                it->second->BroadCastMessage(broadcast);
            }
            else if (v == 2 || v == 3) {
                std::cout << "[3]" << std::endl;
                std::string error = "(482) [" + username + "] " "[" + channel_N + "] :You're not a channel operator \n";
                std::cout << RED << std::endl;
                send(UserSocket, error.c_str(), error.length(), 0);
                std::cout << RESET << std::endl;
            }
        }
        if (v != 0 && !N_topic.empty() && !it->second->isProtectedTopic()) {
            std::cout << "[4]" << std::endl;
			it->second->setTopic(N_topic);
            std::string broadcast = "(332) TOPIC " + channel_N + " :" + N_topic + "\n";
            // send(UserSocket, broadcast.c_str(), broadcast.length(), 0);
            it->second->BroadCastMessage(broadcast);
		}
        else if (v != 0 && N_topic.empty() && !two_dots)
        {
            std::cout << "[5]" << std::endl;
            if (it->second->getTopic().empty())
            {
                std::string error = "(331) [" + username + "] " "[" + channel_N + "] :No topic is set\n";
                send(UserSocket, error.c_str(), error.length(), 0);
            }
            else {
                std::string N_topic = "(332) [" + username + "] " "[" + channel_N + "] :" + it->second->getTopic() + "\n";
                send(UserSocket, N_topic.c_str(), N_topic.length(), 0);
            }
        }
        else if (v != 0 && N_topic.empty() && two_dots)
        {
            it->second->setTopic("");
            std::string broadcast = "(332) TOPIC " + channel_N + " :" + N_topic + "\n";
            it->second->BroadCastMessage(broadcast);
        }
        else if (v == 0)
        {
            std::string error = "(442) [" + username + "] " "[" + channel_N + "] :You're not on that channel\n";
            send(UserSocket, error.c_str(), error.length(), 0);
        }
	}
	else
	{
        std::cout << "[6]" << std::endl;
        std::string error = "(403) [" + username + "] " "[" + channelName + "] :No such channel\n";
		send(UserSocket, error.c_str(), error.length(), 0);
	}
}

void    Database::HandleMode(std::string data, int UserSocket)
{
	bool addMode = true;
	std::stringstream ss(data);

    std::cout << "currently in MODE" << std::endl;
	std::string UserName = GetUserBySocket(UserSocket);
	std::string channelName = ss.str().substr(5, ss.str().find(' ', 5) - 5);
    std::string channel_N = ExtractChannelName(channelName);
    if (channel_N.empty()) {
        std::string error = "(403) [" + UserName + "] " "[" + channelName + "] :No such channel\n";
        send(UserSocket, error.c_str(), error.length(), 0);
        return ;
    }
	std::string mode = ss.str().substr(ss.str().find_first_not_of(' ', 5), ss.str().find(' ') - 5);
    std::cout << "UserName : [" << UserName <<  "] channelName : [" << channelName << "] mode : [" << mode << "]" << std::endl;
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
	for (size_t i = 0; i < mode.size(); i++) {
		char c = mode[i];
		if (c == '+' || c == '-') {
			addMode = (c == '+');
		}
		else {
			applyModeChange(c, addMode, it->second, UserName);
		}
	}
}

std::string Database::ExtractChannelName(std::string input) {
    std::cout << "input is : " << input << std::endl;
    size_t pos;

    pos = input.find_first_of('#');
    if (pos == std::string::npos || input[0] != '#') {
        return "";
    }
    pos = input.find_first_not_of('#');
    if (pos == std::string::npos) {
        return "";
    }
    return input.substr(pos);
}

std::string Database::ExtractTopic(std::string input, bool *two_dots) {
    size_t pos;

    pos = input.find_first_of(':');
    if (pos == std::string::npos || input[0] != ':') {
        *two_dots = false;
        return "";
    }
    *two_dots = true;
    pos = input.find_first_not_of(':');
    if (pos == std::string::npos) {
        return "";
    }
    return input.substr(pos);
}