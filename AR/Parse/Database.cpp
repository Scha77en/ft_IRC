#include "Database.hpp"
#include <iostream>

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
    std::stringstream Respond;

    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->ChannelList(name))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine)
            {
                // if (!ParseUserInput(data, socket)) {
				    Respond.str("");
                    Respond << BLUE << "@" + username << GREEN << " [" << name << "] : " << RESET << data << std::endl;
                    output = Respond.str();
                    std::cout << "Output recieved in Channel " << "[" << data << "]" << std::endl;
                    send(socket, output.c_str(), output.length(), 0);
                // }
            }
        }
    }
}

void Database::StartCommunication(int UserSocket, string data)
{
    string output;
    std::stringstream Respond;

    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);
    string name = user->GetChannelName(user->GetChannelID()); 

    service->DisplayMessages(data, name, username);

    //std::cout << "Connected : " << username  << " to " << name << std::endl;
    //send(UserSocket, 0, 0, 0);
}

bool Database::ParseUserInput(string data, int UserSocket)
{
    string args;
    string command;

    GetCommand(data.c_str(), command, args);

	Database *service = Database::GetInstance();
    std::string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    if (user->GetConnection())
        service->StartCommunication(UserSocket, data);
    if (command == "JOIN")
    {
        std::cout << "command : [" << command << "] args : [" << args << "]" << std::endl;
		if (service->GetChannel(args) == undefine)
		{
			service->AddChannel(args, new Channel());
			string welcome = "Welcome @" + username + " to " + args + "\n";
			send(UserSocket, welcome.c_str(), welcome.length(), 0);
		}
        // service->AddChannel(args, new Channel());
        string username = service->GetUserBySocket(UserSocket);
        Client *user = service->GetClient(username);
        user->SetConnection(TRUE);
        user->SaveChannel(args);
        user->ActiveInChannel(args);

        // string welcome = "Welcome @" + username + " to " + args + "\n";
        // send(UserSocket, welcome.c_str(), welcome.length(), 0);
		service->PrintChannels();
        return true;
    }
	if (command == "TOPIC")
	{
        std::cout << "command : [" << command << "] args : [" << args << "]" << std::endl;
		std::string channelName = args.substr(0, args.find(' '));
		std::string topic = args.substr(args.find(' ') + 1);
		std::map<std::string, Channel *>::iterator it = channels.find(channelName);
		if (it != channels.end())
		{
            std::cout << "GOT INTO TOPIC" << std::endl;
            int v = it->second->DoesClientExist(username);
			if (v != 0 && !topic.empty() && it->second->isProtectedTopic()) {
                if (v == 1) {
                    it->second->setTopic(topic);
                    std::string broadcast = "TOPIC " + channelName + " :" + topic + "\n";
                    it->second->BroadCastMessage(broadcast);
                }
                else if (v == 2 || v == 3) {
                    std::string error = "[" + username + "] " "[" + channelName + "] :You're not a channel operator \n";
                    std::cout << RED << std::endl;
                    send(UserSocket, error.c_str(), error.length(), 0);
                    std::cout << RESET << std::endl;
                }
            }
	        if (v != 0 && !topic.empty() && !it->second->isProtectedTopic()) {
				it->second->setTopic(topic);
                std::string broadcast = "TOPIC " + channelName + " :" + topic + "\n";
                it->second->BroadCastMessage(broadcast);
			}
            if (v != 0 && topic.empty())
            {
                std::string topic = "[" + username + "] " "[" + channelName + "] :" + it->second->getTopic() + "\n";
                send(UserSocket, topic.c_str(), topic.length(), 0);
            }
		}
		else
		{
            std::string error = "[" + username + "] " "[" + channelName + "] :No such channel\n";
			send(UserSocket, error.c_str(), error.length(), 0);
		}
        return true;
	}
	else if (command == "MODE")
	{
		bool addMode = true;

		std::string channelName = args.substr(0, args.find(' '));
		std::string mode = args.substr(args.find(' ') + 1);
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
        return true;
	}
    return false;
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
        std::cout << it->first << std::endl;
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
	return channels[name];
}

// void Database::handleTopic(std::string name)
// {
// 	std::cout << "HANDLING TOPIC " << name << std::endl;
// 	std::string channelName = name.substr(0, name.find(' '));
// 	std::string topic = name.substr(name.find(' ') + 1);
// 	std::cout << "Channel Name = " << channelName << std::endl;
// 	std::cout << "Topic = " << topic << std::endl;
// 	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
// 	if (it != channels.end())
// 	{
// 		std::cout << YELLOW "Topic to set : " << topic << std::endl;
// 		it->second->setTopic(topic);
// 		std::cout << YELLOW "TOPIC IS SET = " << it->second->getTopic() << GREEN << std::endl;
// 	}
// 	else
// 	{
// 		std::cout << RED "CHANNEL NOT FOUND" << GREEN << std::endl;
// 	}
	// Channel *channel = channels[channelName];
	// channel->setTopic(topic);
	// std::cout << YELLOW "TOPIC IS SET = " << channel->getTopic() << GREEN << std::endl;
// }

// void	Database::handleMode(char buffer[1024])
// {
// 	bool addMode = true;

// 	std::string data = buffer + 5;
// 	std::string channelName = data.substr(0, data.find(' '));
// 	std::string mode = data.substr(data.find(' ') + 1);
// 	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	
// 	for (size_t i = 0; i < mode.size(); i++) {
// 		char c = mode[i];
// 		if (c == '+' || c == '-') {
// 			addMode = (c == '+');
// 		}
// 		else {
// 			applyModeChange(c, addMode, it->second);
// 		}
// 	}
// }

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

Database::~Database()
{
	delete DB;
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