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
    string output = ":" + IP + " 475 " + username + " #" + name + " :Cannot join channel (+k) - bad key\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 475 adrian #test :Cannot join channel (+k) - bad key
*/

void Protection471(string name, int UserSocket, string username, string IP)
{
    string output = ":" + IP + " 471 " + username + " #" + name + " :Cannot join channel (+l)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 471 alice #test :Cannot join channel (+l)
*/

void Protection474(string name, int UserSocket, string username, string IP)
{
    string output = ":" + IP + " 474 " + username + " #" + name + " :Cannot join channel (+b)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 474 alice #test :Cannot join channel (+b)
*/

void Protection473(string name, int UserSocket, string username, string IP)
{
    string output = ":" + IP + " 473 " + username + " #" + name + " :Cannot join channel (+i)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

/*
S <-   :irc.example.com 473 alice #test :Cannot join channel (+i)
*/

void xProtection403(string command, int UserSocket, string username)
{
    string output = "403 " + username + " " + command + " :Unknown command\n";
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
        Respond.str("");
        Respond << "461)" << BLUE << " @" + username << " JOIN :" << RED << "Not enough parameters " << RESET << std::endl;
        output = Respond.str();
        send(UserSocket, output.c_str(), output.length(), 0);
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
        Channel *channel = service->GetChannel(it->first);
        if (channel != undefine)
        {
            channel->BanMember("username"); // Delete after for test Only !
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
            /*
            if (channel->isInviteOnly())
            {
                Protection473(it->first, UserSocket, username, IP);
                continue;
            }
            */
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
    string output = "421 " + username + " " + command + " :Unknown command\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void Protection403(string command, int UserSocket, string username)
{
    string output = "403 " + username + " " + command + " :Unknown command\n";
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
    else if (command == "MODE" || command == "mode" || command == "PONG" || command == "pong")
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

/*
:u0op00!~tyue@197.230.30.146 PRIVMSG u0op : HELLO

*/
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
    string hostname = "";
    string USER = "";
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->first == username)
        {
           hostname = it->second->GetClientIP();
           USER = it->second->GetName();
        }
    }
    output = ":"+ username + "!~" + USER + "@" + hostname + " PRIVMSG " + name + " " + data + "\n";
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
            service->DisplayMessages(msg, List[i].substr(1), username, UserSocket);
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
            if (socket > undefine && socket != UserSocket)
            {
                it->second->GetName();
                output = ":"+username+ "!~"+it->second->GetName()+"@" + it->second->GetClientIP() + " PRIVMSG #"+name+" "+data+"\n";
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

/*
:dan!~h@localhost PRIVMSG #coolpeople :
*/

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
