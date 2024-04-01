#include "Database.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

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

void Database::SetServerIP(struct in_addr host)
{
	this->server_ip = host;
}

string Database::GetServerIP(void)
{
	string HOST(inet_ntoa(server_ip));
	return (HOST);
}

void Database::NoticeUserHasJoined(string name, string username, int UserSocket, string IP)
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
				Respond << ":" + username + "!~" + username.substr(0,1) +  "@" + IP + " JOIN #" + name << std::endl;
				output = Respond.str();
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
	Respond << ":" + GetServerIP() + " 366 " + username << " #" + name + " :End of /NAMES list." << RESET << std::endl;
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

void Protection475(string name, string username, int UserSocket, string IP)
{
	string output = ":irc.1337.com" + IP + " 475 " + username + " #" + name + " :Cannot join channel (+k) - bad key\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 475 adrian #test :Cannot join channel (+k) - bad key
*/

void Protection471(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 471 " + username + " #" + name + " :Cannot join channel (+l)\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 471 alice #test :Cannot join channel (+l)
*/

void Protection474(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 474 " + username + " #" + name + " :Cannot join channel (+b)\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 474 alice #test :Cannot join channel (+b)
*/

void Protection473(string name, int UserSocket, string username, string IP)
{
	string output = ":irc.1337.com" + IP + " 473 " + username + " #" + name + " :Cannot join channel (+i)\n";
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

void ERR_TOOMANYCHANNELS_405(string name, int UserSocket, string username)
{
	string output = ":irc.1337.com 405 " + username + " #" + name + " :You have joined too many channels\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NEEDMOREPARAMS_461(string name, int UserSocket, string username)
{
	string output = ":irc.1337.com 461 " + username + " " + name + " :Not enough parameters\n";
	send(UserSocket, output.c_str(), output.length(), 0);
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
		ERR_NEEDMOREPARAMS_461("JOIN", UserSocket, username);
		return ;
	}
	if (Potection403(args))
	{
		xProtection403(args, UserSocket, username);
		return ;
	}
	string IP = user->GetClientIP();
	SYSTEM_KEYVAL channels = parseChannels(args, UserSocket, username);
	for (it = channels.begin(); it != channels.end();it++)
	{
		if (user->LimitedChannels())
		{
			ERR_TOOMANYCHANNELS_405(it->first, UserSocket, username);
			break;
		}
		Channel *channel = service->GetChannel(it->first);
		if (channel != undefine)
		{
			//channel->BanMember("username"); // Delete after for test Only !
			if (channel->GetSecretKey() != it->second)
			{
				Protection475(it->first, username, UserSocket, IP);
				continue;
			}
			if (channel->GetLimit() != -1)
			{
				bool OutOfBound = channel->CountInvited() + channel->CountMembers() + channel->CountAdmins() + 1 > channel->GetLimit();
				if (OutOfBound)
				{
					Protection471(it->first, UserSocket, username, IP);
					continue;
				}
			}
			if (channel->UserIsBanned(username))
			{
				Protection474(it->first, UserSocket, username, IP);
				continue;
			}
			if (channel->isInviteOnly())
			{
				int state = channel->DoesClientExist(username);
				
				Protection473(it->first, UserSocket, username, IP);
				continue;
			}
		}
		if (channel == undefine)
		{
			channel = new Channel(it->first, it->second);
			channel->SetSymbol("=");
			service->AddChannel(it->first, channel);
		}
		else
			channel->SetSymbol("@");
		user->SaveChannel(it->first);
		user->ActiveInChannel(it->first);
		if (channel->FirstCreation())
			channel->addAdmin(username);
		else
			channel->addMember(username);
		NoticeUserHasJoined(it->first, username, UserSocket, IP);
	}
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

	if (command == "JOIN" || command == "join")
		HandelMultiChannel(data, UserSocket);
	else if (command == "PRIVMSG" || command == "privmsg")
		service->StartCommunication(UserSocket, data.substr(8));
	else if (command == "TOPIC" || command == "topic")
		service->HandleTopic(data, UserSocket);
	else if (command == "INVITE" || command == "invite")
		HandleInvite(data, UserSocket);
	else if (command == "MODE" || command == "mode") 
		service->HandleMode(data, UserSocket);
	else if (command == "PONG" || command == "pong")
		return ;
	else
		Protection421(command, UserSocket, username);
}

STORE GetSendingList(string data, string &message)
{
	STORE List;

	size_t pos = data.find("::");
	if (pos != std::string::npos) 
		data[pos] = ' ';

	std::string Ranges;
	std::string token;
	std::stringstream Obj(data);
	
	std::getline(Obj, Ranges, ' ');
	std::getline(Obj, message);
	
	std::stringstream Object(Ranges);
	std::cout << "Ranges : [" + Ranges << "] message : [" + message + "\n";
	while (std::getline(Object, token, ','))
	{
		token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
		List.push_back(token);
	}
	return (List);
}

void ERR_NORECIPIENT_411(string username, int UserSocket)
{
	string output = ":irc.1337.com 411 " + username + " :No recipient given PRIVMSG\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHCHANNEL_403(string username, string target, int UserSocket)
{
	string output = ":irc.1337.com 403 " + username + " " + target + " :No such channel\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CANNOTSENDTOCHAN_404(string username, string target, int UserSocket)
{
	string output = ":irc.1337.com 403 " + username + " " + target + " :Cannot send to channel\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHNICK(string username, string target, int UserSocket)
{
	string output = ":irc.1337.com 401 " + username + " " + target + " :No such nick/channel\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOTEXTTOSEND_412(string username, int UserSocket)
{
	string output = ":irc.1337.com 412 " + username + " :No text to send\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void RPL_AWAY_301(string username, string target, string msg ,int UserSocket)
{
	string output = ":irc.1337.com 301 " + username + " " + target + " " + msg + "\n";
	send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::PRIVMessages(string data, string name, string username)
{
	int senderSocket = undefine;
	int socketFailed = GetUserSocket(name);

	string USER = "";
	string hostname = "";
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first == username)
		{
		   hostname = it->second->GetClientIP();
		   USER = it->second->GetName();
		   senderSocket = it->second->GetSocket();
		   break;
		}
	}
	if (socketFailed == undefine)
	{
		ERR_NOSUCHNICK(username, name, senderSocket);
		return ;
	}
	string output = ":"+ username + "!~" + USER + "@" + hostname + " PRIVMSG " + name + " " + data + "\n";
	send(socketFailed, output.c_str(), output.length(), 0);
}


void Database::StartCommunication(int UserSocket, string data)
{
	string msg;
	STORE List;
	Database *service = Database::GetInstance();
	string username = service->GetUserBySocket(UserSocket);

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
			service->DisplayMessages(msg, List[i].substr(1), username, UserSocket);
		else
			service->PRIVMessages(msg, List[i], username);
	}
}

void Database::DisplayMessages(string data, string name, string username, int UserSocket)
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
		ERR_NOSUCHCHANNEL_403(username, name, UserSocket);
		return ;
	}
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end() && is_out; ++it)
	{
		if (it->second->ChannelList(name))
		{
			int socket = it->second->GetSocket();
			if (socket > undefine && socket != UserSocket)
			{
				it->second->GetName();
				output = ":"+username+ "!~"+it->second->GetName()+"@" + it->second->GetClientIP() + " PRIVMSG #"+name+" "+data+"\n";
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
		std::cout << "=> " << it->first << std::endl;
}

string Database::GetUserBySocket(int UserSocket)
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->GetSocket() == UserSocket)
			return it->first;
	}
	return ("");
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
// *************************************************************************


void    Database::HandleTopic(std::string data, int UserSocket)
{
	Database *database = Database::GetInstance();
	std::string username = database->GetUserBySocket(UserSocket);
	std::string N_topic;
	std::string command;
	std::string channel_N;
	bool    two_dots = false;
	std::stringstream  ss(data);

	ss >> command >> channel_N;
	std::getline(ss, N_topic);

	std::cout << "command : " << command << std::endl;
	std::cout << "channel_N : " << channel_N << std::endl;
	std::cout << "N_topic : " << N_topic << std::endl;

	// condition 0 : "TOPIC" --> need more params
	if (channel_N.empty()) {
		ERR_461_NEEDMOREPARAMS(username, command, UserSocket);
		return ;
	}
	channel_N = ExtractChannelName(channel_N);

	if (channel_N.empty()) {
		ERR_403_NOSUCHCHANNEL(username, channel_N, UserSocket);
		return ;
	}
	std::map<std::string, Channel *>::iterator it = channels.find(channel_N);

	N_topic = ExtractTopic(N_topic, &two_dots);
	std::cout << "N_topic extracted : [" << N_topic << "]" << std::endl;
	// condition 1 : "TOPIC #channel" -> get the topic
	if (N_topic.empty() && !two_dots) {
		// --- if topic is empty then the client is asking for the topic ---
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, channel_N, UserSocket);
			return ;
		}
		else {
			if (it->second->getTopic().empty())
				RPL_NOTOPIC_331(username, channel_N, UserSocket);
			else
				RPL_TOPIC_332(username, channel_N, it->second->getTopic(), UserSocket);
		}
	}
	else if (N_topic.empty() && two_dots) {
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, channel_N, UserSocket);
			return ;
		}
		else if (state == 1) {
			it->second->setTopic("");
			std::string broadcast = RPL_TOPIC(username, channel_N, "");
			it->second->BroadCastMessage(broadcast);
		}
		else if (it->second->isProtectedTopic() && (state == 2 || state == 3)) {
			ERR_482_CHANOPRIVSNEEDED(username, channel_N, UserSocket);
			return ;
		}
		else {
			it->second->setTopic("");
			std::string broadcast = RPL_TOPIC(username, channel_N, "");
			it->second->BroadCastMessage(broadcast);
		}
	}
	// condition 2 : "TOPIC #channel :" -> set the topic to empty
	else if (!N_topic.empty()) {
		int state = it->second->DoesClientExist(username);
		if (state == 0 || state == 3) {
			ERR_442_NOTONCHANNEL(username, channel_N, UserSocket);
			return ;
		}
		else if (state == 1) {
			it->second->setTopic(N_topic);
			std::string broadcast = RPL_TOPIC(username, channel_N, N_topic);
			it->second->BroadCastMessage(broadcast);
		}
		else if (it->second->isProtectedTopic() && (state == 2 || state == 3)) {
			ERR_482_CHANOPRIVSNEEDED(username, channel_N, UserSocket);
			return ;
		}
		else {
			it->second->setTopic(N_topic);
			std::string broadcast = RPL_TOPIC(username, channel_N, N_topic);
			it->second->BroadCastMessage(broadcast);
		}
	}
}

void    Database::HandleMode(std::string data, int UserSocket)
{
	bool addMode = true;
	std::stringstream ss(data);

	std::string UserName = GetUserBySocket(UserSocket);
	std::string	command, channelName, modes, remain;

	ss >> command >> channelName >> modes;

	std::cout << "command : " << command << std::endl;
	std::cout << "channelName : " << channelName << std::endl;
	std::cout << "modes : " << modes << std::endl;

	std::vector<std::string> m_args;

	while (ss >> remain)
		m_args.push_back(remain);

	if (channelName.empty()) {
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
	else if (state == 2) {
		ERR_482_CHANOPRIVSNEEDED(UserName, channelName, UserSocket);
		return ;
	}

	if (modes.empty()) {
		RPL_324_CHANNELMODEIS(UserName, channelName,  UserSocket);
		return ;
	}

	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
	for (size_t i = 0; i < modes.size(); i++) {
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
	std::string command, username, channelName, target;

	std::stringstream ss(data);
	ss >> command >> target >> channelName;
	username = GetUserBySocket(UserSocket);

	if (channelName.empty()) {
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
		state = it->second->DoesClientExist(target);
		if (state == 0 || state == 3) {
			RPL_341_INVITING(username, target, channelName, UserSocket);
			return ;
		}
		else {
			ERR_443_USERONCHANNEL(username, target, channelName, UserSocket);
			return ;
		}
	}
}

std::string Database::ExtractChannelName(std::string input) {
	size_t pos;

	if (input.empty() || input[0] != '#') {
		return "";
	}
	pos = input.find_first_not_of('#');
	if (pos == std::string::npos) {
		return "";
	}
	std::string Channel = input.substr(pos, input.length() - pos);

	std::cout << YELLOW "Extracted Channel is : " << Channel << RESET << std::endl;
	if (!DoesChannelExist(Channel))
		return "";
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
	*two_dots = true;
	return input.substr(pos, input.length() - pos);
}

void	Database::applyModeChange(char mode, bool addMode, Channel *channel, std::string UserName, std::vector<std::string> &m_args)
{
	std::string broadcast;
	int TargetSocket = 0;
	int UserSocket = GetUserSocket(UserName);
	std::cout << "applyModeChange : --> " << mode << std::endl;
	switch (mode) {
		case 'i':
			channel->setInviteOnly(addMode);
			if (addMode)
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " +i\n";
			else
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " -i\n";
			channel->BroadCastMessage(broadcast);
			break;
		case 't':
			channel->setProtectedTopic(addMode);
			if (addMode)
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " +t\n";
			else
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " -t\n";
			channel->BroadCastMessage(broadcast);
			break;
		case 'l':
			if (channel->setUserLimit(m_args, UserName, addMode)) {
				if (addMode)
					broadcast = "324 " + UserName + " #" + channel->ChannelName() + " +l\n";
				else
					broadcast = "324 " + UserName + " #" + channel->ChannelName() + " -l\n";
				channel->BroadCastMessage(broadcast);
			}
			break;
		case 'k':
			if (channel->setKey(m_args, addMode, UserName)) {
			if (addMode)
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " +k\n";
			else
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " -k\n";
			channel->BroadCastMessage(broadcast);
			}
			break;
		case 'o':
			TargetSocket = GetUserSocket(m_args[0]);
			if (channel->SetOperator(UserName, addMode, m_args)) {
			if (addMode)
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " +o\n";
			else
				broadcast = "324 " + UserName + " #" + channel->ChannelName() + " -o\n";
			send(TargetSocket, broadcast.c_str(), broadcast.length(), 0);
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
	std::string error = RPL_INVITING(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_443_USERONCHANNEL(std::string UserName, std::string target, std::string channel, int UserSocket)
{
	std::string error = ERR_USERONCHANNEL(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}

// -----------------------------------------------------------

Database::~Database()
{
	for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
		delete it->second;
	for (SYSTEM_CHANNEL::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
}