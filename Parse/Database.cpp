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
