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

void Database::SetServerIP(struct in_addr host)
{
    this->server_ip = host;
}

string Database::GetServerIP(void)
{
    string HOST(inet_ntoa(server_ip));
    return (HOST);
}

void Database::HandelMultiChannel(string data, int UserSocket)
{
    string args;
    SYSTEM_KEYVAL::iterator it;

    CleanInput(data, ' ');
    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);
    if (Protection(data) || data.empty())
    {
        ERR_NEEDMOREPARAMS_461("JOIN",UserSocket, username);
        return ;
    }
    string IP = user->GetClientIP();
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
        string EXIST(it->first);
    
        if (EXIST.empty() || EXIST.back() != '\0')
            EXIST.push_back('\0');
        
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
            if (channel->GetSecretKey() != it->second)
            {
                ERR_BADCHANNELKEY_475(EXIST, username, UserSocket, IP);
                continue;
            }
            if (channel->GetLimit() != -1)
            {
                bool OutOfBound = channel->CountInvited() + channel->CountMembers() + channel->CountAdmins() + 1 > channel->GetLimit();
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
            if (channel->isInviteOnly())
            {
                ERR_INVITEONLYCHAN_473(EXIST, UserSocket, username, IP);
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

        if (EXIST.empty() || EXIST.back() != '\0')
            EXIST.push_back('\0');

		SYSTEM_CHANNEL::iterator xit;
        for (xit = service->channels.begin(); xit != service->channels.end() && !is_inChannel; ++xit)
        {
            string Detected(xit->first);

            if (Detected.empty() || Detected.back() != '\0')
                Detected.push_back('\0');
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

    if (ChannelName.empty() || ChannelName.back() != '\0')
        ChannelName.push_back('\0');
    if (TrgetUsername.empty() || TrgetUsername.back() != '\0')
        TrgetUsername.push_back('\0');

    bool not_found = undefine;
    string IP = user->GetClientIP();
    SYSTEM_CHANNEL::iterator xit;
    for (xit = service->channels.begin(); xit != service->channels.end(); ++xit)
    {
        string Detected(xit->first);

        if (Detected.empty() || Detected.back() != '\0')
            Detected.push_back('\0');
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

void Database::NoticeUserKICK(string ChannelName, string username, string IP, string target, string msg)
{
    string output;

    Database *service = Database::GetInstance();
    Client *user = service->GetClient(username);

    if (msg.empty() || (msg.length() <= 2 && msg.length() > 0 && msg[0] == ':' && msg[1] == '\0'))
        output = ":" + username + "!~" + user->GetName() +  "@" + IP + " KICK " + ChannelName + " " + target + " :" + target + "\n";
    else
    {
        size_t pos = msg.find(":");
        if (pos == std::string::npos)
        {
            size_t posX = msg.find(" ");
            msg = ":" + msg.substr(0, posX);
        }
        output = ":" + username + "!~" + user->GetName() +  "@" + IP + " KICK " + ChannelName + " " + target + " " + msg + "\n";
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
        output = ":" + username + "!~" + user->GetName() +  "@" + IP + " PART " + ChannelName + " " + msg + "\n";

    }
    else
        output = ":" + username + "!~" + user->GetName() +  "@" + IP + " PART " + ChannelName + "\n";
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

void Database::NoticeUserHasJoined(string name, string username, int UserSocket, string IP)
{
    string output;

    Database *service = Database::GetInstance();
    Client *user = service->GetClient(username);

    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->ChannelList(name))
        {
            int socket = it->second->GetSocket();
            if (socket > undefine)
            {
                output = ":" + username + "!~" + user->GetName() +  "@" + IP + " JOIN " + name + "\n";
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
    output = ":irc.1337.com 366 " + username + " " + name + " :End of /NAMES list.\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::ParseUserInput(string data, int UserSocket)
{
    string args;
    string command;

	Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);

    CleanInput(data, ' ');
    std::stringstream cmd(data);
    cmd >> command;
    std::getline(cmd, args);

    std::cout << "command [" + command + "]\n"; 
    std::cout << "args [" + args + "]\n"; 

    if (command == "JOIN" || command == "join")
        service->HandelMultiChannel(args, UserSocket);
    else if (command == "PRIVMSG" || command == "privmsg")
        service->StartCommunication(UserSocket, args);
    else if (command == "PART" || command == "part")
        return service->HandelMultiPART(args, UserSocket);
    else if (command == "MODE" || command == "mode" || command == "PONG" || command == "pong")
        return ;
    else if (command == "KICK" || command == "kick")
        service->HandelKick(args, UserSocket);
    else
        ERR_UNKNOWNCOMMAND_421(command, UserSocket, username);
}

void Database::PRIVMessages(string data, string name, string username)
{
    int senderSocket = undefine;
    int socketFailed = GetUserSocket(name);

    string USER = "";
    string hostname = "";

    if (username.empty() || username.back() != '\0')
        username.push_back('\0');
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        string CHECK(it->first);
        if (CHECK.empty() || CHECK.back() != '\0')
            CHECK.push_back('\0');
        if (CHECK == username)
        {
           hostname = it->second->GetClientIP();
           USER = it->second->GetName();
           senderSocket = it->second->GetSocket();
           break;
        }
    }
    if (socketFailed == undefine)
    {
        ERR_NOSUCHNICK_401(username, name, senderSocket);
        return ;
    }
    size_t pos = data.find(":");
    if (pos == std::string::npos)
    {
        size_t posX = data.find(" ");
        data = ":" + data.substr(0, posX);
    }
    string output = ":"+ username + "!~" + USER + "@" + hostname + " PRIVMSG " + name + " " + data + "\n";
    send(socketFailed, output.c_str(), output.length(), 0);
}


void Database::StartCommunication(int UserSocket, string data)
{
    string msg;
    STORE List;
    
    CleanInput(data, ' ');
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
        if (List[i].empty() || List[i].back() != '\0')
            List[i].push_back('\0');
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
    int socketFailed = undefine;

    if (name.empty() || name.back() != '\0')
        name.push_back('\0');
    if (username.empty() || username.back() != '\0')
        username.push_back('\0');
    for (SYSTEM_CLIENT::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        string CHECK(it->first);
        if (CHECK.empty() || CHECK.back() != '\0')
            CHECK.push_back('\0');
        if (it->second->ChannelList(name) && CHECK == username)
            is_out = 1;
        if (CHECK == username)
            socketFailed = it->second->GetSocket();
    }
    if (is_out == undefine)
    {
        ERR_NOSUCHCHANNEL_403(username, name, UserSocket);
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
                it->second->GetName();
                output = ":"+username+ "!~"+it->second->GetName()+"@" + it->second->GetClientIP() + " PRIVMSG "+name+" "+data+"\n";
                send(socket, output.c_str(), output.length(), 0);
            }
        }
    }
}

void Database::AddChannel( std::string& name, Channel* channel)
{
	channels[name] = channel;
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

Channel* Database::GetChannel(std::string name)
{
    
    for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        string EXIST(it->first);

        if (EXIST.empty() || EXIST.back() != '\0')
            EXIST.push_back('\0');
        if (name == EXIST)
            return channels[EXIST];
    }
	return undefine;
}


