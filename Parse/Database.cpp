#include "Database.hpp"

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
                Respond << BLUE << "@" + username << GREEN << " [" << name << "] : " << RESET << data << std::endl;
                output = Respond.str();
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Database::NoticeUserHasJoined(string name, string username)
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

void Database::StartCommunication(int UserSocket, string data)
{
    string name = ""; // Channel Name
    string msg = "";

    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);
    //string name = user->GetChannelName(user->GetChannelID());

    size_t colonPos = data.find(' ');
    name = data.substr(0, colonPos);
    if (colonPos != std::string::npos)
        msg = data.substr(colonPos + 1);
    std::cout << "Channel Name : [" << name << "] Msg : [" << msg << "]"<< std::endl;
    if (name == "QUIT")
    {
        NoticeUserLogout(msg, username);
        user->ChannelLogout(msg);
    }
    else
        service->DisplayMessages(msg, name, username);
}

SYSTEM_KEYVAL parseChannels(string &input) 
{
    string token;
    SYSTEM_KEYVAL result;
    std::istringstream iss(input);

    while (std::getline(iss, token, ',')) 
    {
        size_t pos = token.find(':');
        std::string channel = token.substr(0, pos);
        std::string key = (pos != std::string::npos) ? token.substr(pos + 1) : ""; // If ':' is not found, key is empty
        result.push_back(std::make_pair(channel, key));
    }
    return result;
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
    // else send error msg in client socket

    SYSTEM_KEYVAL channels = parseChannels(args);
    for (it = channels.begin(); it != channels.end();it++)
    {
        Channel *channel = service->GetChannel(it->first);
        if (channel != undefine && channel->GetSecretKey() != it->second)
        {
            Respond.str("");
            Respond << BLUE << it->first << RED << " is Protected ! " << RESET << std::endl;
            output = Respond.str();
            send(UserSocket, output.c_str(), output.length(), 0);
            continue;
        }
        channel = new Channel(it->first, it->second);
        user->SetConnection(TRUE);
        user->SaveChannel(it->first);
        user->ActiveInChannel(it->first);
        service->AddChannel(it->first, channel);
        if (channel->FirstCreation())
            channel->addAdmin(username);
        else
            channel->addMember(username);
        NoticeUserHasJoined(it->first, username);
    }
}

void Database::ParseUserInput(string data, int UserSocket)
{
    string args;
    string command;

    GetCommand(data.c_str(), command, args);

	Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    if (user->GetConnection())
        service->StartCommunication(UserSocket, data);
    else if (command == "J")
        HandelMultiChannel(data, UserSocket);
}

void Database::AddChannel(const std::string& name, Channel* channel)
{
	channels[name] = channel;
	PrintChannels();
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

Channel* Database::GetChannel(const std::string& name)
{
    for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (name == it->first)
            return channels[name];
    }
	return undefine;
}

/*

void Database::RemoveChannel(const std::string& name)
{
	channels.erase(name);
}

void Database::RemoveClient(const std::string& name)
{
	clients.erase(name);
}

Database::~Database()
{
	delete Database_;
}

void Database::parce_user_data(char buffer[1024])
{
	Database *sing = Database::GetInstance();
	if (std::strncmp(buffer, "TOPIC", 5) == 0)
	{
		std::cout << "TOPIC HAS BEEN ENCOUNTERED" << std::endl;
		sing->handleTopic(buffer);
	}
	else if (std::strncmp(buffer, "JOIN", 4) == 0)
	{
		std::cout << "JOIN HAS BEEN ENCOUNTERED" << std::endl;
		sing->AddChannel(buffer + 5, new Channel());
	}
	else if (std::strncmp(buffer, "MODE", 4) == 0)
	{
		sing->handleMode(buffer);
	}
}

void Database::handleTopic(char buffer[1024])
{
	std::string name = buffer + 6;
	std::cout << "HANDLING TOPIC " << name << std::endl;
	std::string channelName = name.substr(0, name.find(' '));
	std::string topic = name.substr(name.find(' ') + 1);
	std::cout << "Channel Name = " << channelName << std::endl;
	std::cout << "Topic = " << topic << std::endl;
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	if (it != channels.end())
	{
		std::cout << YELLOW "Topic to set : " << topic << std::endl;
		it->second->setTopic(topic);
		std::cout << YELLOW "TOPIC IS SET = " << it->second->getTopic() << GREEN << std::endl;
	}
	else
	{
		std::cout << RED "CHANNEL NOT FOUND" << GREEN << std::endl;
	}
	// Channel *channel = channels[channelName];
	// channel->setTopic(topic);
	// std::cout << YELLOW "TOPIC IS SET = " << channel->getTopic() << GREEN << std::endl;
}

void	Database::handleMode(char buffer[1024])
{
	bool addMode = true;

	std::string data = buffer + 5;
	std::string channelName = data.substr(0, data.find(' '));
	std::string mode = data.substr(data.find(' ') + 1);
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
	for (size_t i = 0; i < mode.size(); i++) {
		char c = mode[i];
		if (c == '+' || c == '-') {
			addMode = (c == '+');
		}
		else {
			applyModeChange(c, addMode, it->second);
		}
	}
}

void	Database::applyModeChange(char mode, bool addMode, Channel *channel)
{
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
			std::cout << "Operator" << std::endl;
			break;
		default:
			break;
	}
}
*/

/*
DataBase::DataBase() {}

DataBase::~DataBase() {}

int DataBase::UserIndex(int NewClientSocket)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].socket == NewClientSocket)
            return (i);
    }
    return (-1);
}

int DataBase::UserSocket(string username)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].name == username)
            return (users[i].socket);
    }
    return (-1);
}

bool DataBase::IsOnline(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].online);
}

void DataBase::SetConnection(int NewClientSocket, bool status)
{
    users[UserIndex(NewClientSocket)].online = status;
}

std::string DataBase::FindUser(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].name);
}

bool DataBase::UpdateUserChannel(int NewClientSocket, string &channel)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].socket == NewClientSocket)
        {
            int ChannelIndex = 0;
            for (int x = 0; x < MAX_CHANNEL; x++)
            {
                if (users[i].ChannelID[x].empty())
                    ChannelIndex = x;
                
                if (users[i].ChannelID[x] == channel)
                    return (FALSE);
            }
            users[i].ChannelID[ChannelIndex] = channel;
            return (TRUE);
        }
    }
    return (TRUE);
}

void DataBase::ActiveInChannel(int index, int NewClientSocket)
{
    users[UserIndex(NewClientSocket)].CurrentChannel = index;
}

int DataBase::InChannelID(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].CurrentChannel);
}

void DataBase::UserAddInformation(int ID, int Socket, string Name)
{
    users[ID].id = ID;
    users[ID].name = Name;
    users[ID].socket = Socket;
    for (size_t i = 0; i < MAX_CHANNEL; i++)
    {
        users[ID].ChannelID[i] = "";
        users[ID].Permission[i] = 0;
    }
    users[ID].online = FALSE;
    users[ID].CurrentChannel = 0;
}
*/