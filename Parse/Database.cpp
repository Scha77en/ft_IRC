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
                Respond << ":" + username + "!~" + username.substr(0,1) +  "@" + IP + " JOIN " + name << std::endl;
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
    Respond << ":" + GetServerIP() + " 366 " + username << " " + name + " :End of /NAMES list." << std::endl;
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

void RemoveCharacter(int ASCII, string &target)
{
    int x = 0;
    while (target[x])
    {
        if ((int)target[x] == ASCII)
        {
            target[x] = '\0';
            break;
        }
        x++;
    }
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

    /*
    RangeChannels.erase(std::remove(RangeChannels.begin(), RangeChannels.end(), '\x05'), RangeChannels.end());
    RangeChannels.shrink_to_fit();
    RangeKeys.erase(std::remove(RangeKeys.begin(), RangeKeys.end(), '\x05'), RangeKeys.end());
    RangeKeys.shrink_to_fit();
    */
    for (size_t i = 0; i < RangeChannels.size(); ++i) 
    {
        if (i < RangeKeys.size())
        {
            if (RangeChannels[i][0] == '#')
            {
                std::cout << "lengh : " << RangeChannels[i].length() << std::endl;
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
    SYSTEM_KEYVAL::iterator it;

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
        //it->first.erase(std::remove(it->first.begin(), it->first.end(), '\x05'), it->first.end());
        //it->first.shrink_to_fit();
        string EXIST(it->first);

    
        if (EXIST.empty() || EXIST.back() != '\0')
            EXIST.push_back('\0');
        if (user->LimitedChannels())
        {
            ERR_TOOMANYCHANNELS_405(EXIST, UserSocket, username);
            break;
        }
        if (user->ChannelList(EXIST))
        {
            std::cout << "ALREADY IN CHANNEL : #" <<  EXIST << std::endl;
            continue;
        }
        Channel *channel = service->GetChannel(EXIST);
        if (channel != undefine)
        {
            //channel->BanMember("username"); // Delete after for test Only !
            if (channel->GetSecretKey() != it->second)
            {
                Protection475(EXIST, username, UserSocket, IP);
                continue;
            }
            if (channel->GetLimit() != -1)
            {
                bool OutOfBound = channel->CountInvited() + channel->CountMembers() + channel->CountAdmins() + 1 > channel->GetLimit();
                if (OutOfBound)
                {
                    Protection471(EXIST, UserSocket, username, IP);
                    continue;
                }
            }
            if (channel->UserIsBanned(username))
            {
                Protection474(EXIST, UserSocket, username, IP);
                continue;
            }
            if (channel->isInviteOnly())
            {
                Protection473(EXIST, UserSocket, username, IP);
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

void Database::NoticeUserPART(string ChannelName, string username, int UserSocket, string IP)
{
    string output;
    std::stringstream Respond;

    Database *service = Database::GetInstance();
    Client *user = service->GetClient(username);

    output = ":" + username + "!~" + user->GetName() +  "@" + IP + " PART #" + ChannelName + "\n";
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

void ERR_NOTONCHANNEL_442(string username, int UserSocket, string ChannelName)
{
    string output = ":irc.1337.com 442 " + username + " " + ChannelName + " :You're not on that channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void Database::HandelMultiPART(string data, int UserSocket)
{
    string args;
    SYSTEM_KEYVAL::iterator it;

    Database *service = Database::GetInstance();
    string username = service->GetUserBySocket(UserSocket);
    Client *user = service->GetClient(username);

    if (Protection(data) || data.empty())
    {
        ERR_NEEDMOREPARAMS_461("PART",UserSocket, username);
        return ;
    }
    bool is_inChannel = undefine;
    string IP = user->GetClientIP();
    SYSTEM_KEYVAL ProcessChannels = parseChannels(data, UserSocket, username);
    for (it = ProcessChannels.begin(); it != ProcessChannels.end();it++)
    {
        //std::cout << "-> (CH) : [" + it->first + "]\n";
        //std::cout << "-> (KY): [" + it->second + "]\n";

		SYSTEM_CHANNEL::iterator xit;
        for (xit = service->channels.begin(); xit != service->channels.end() && !is_inChannel; ++xit)
        {
            if (xit->first == it->first)
            {
                is_inChannel = 1;
                xit->second->PartFromChannels(username);
            }
        }
        if (user->DeleteChannel(it->first))
            NoticeUserPART(it->first, username, UserSocket, IP);
        else if (is_inChannel == undefine)
            ERR_NOSUCHCHANNEL_403(username, it->first, UserSocket);
        else
            ERR_NOTONCHANNEL_442(username, UserSocket, it->first);
        is_inChannel = undefine;
    }
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
        HandelMultiChannel(data.substr(4), UserSocket);
    else if (command == "PRIVMSG" || command == "privmsg")
        service->StartCommunication(UserSocket, data.substr(8));
    else if (command == "PART" || command == "part")
        return service->HandelMultiPART(data.substr(4), UserSocket);
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

void ERR_NOSUCHNICK_401(string username, string target, int UserSocket)
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
        ERR_NOSUCHNICK_401(username, name, senderSocket);
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

    std::cout << "socketFailed : " << socketFailed << std::endl;
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

void Database::AddChannel( std::string& name, Channel* channel)
{
    /*
    name.erase(std::remove(name.begin(), name.end(), '\x05'), name.end());
    name.shrink_to_fit();
    */
    //name[name.length() - 1] = '\0';
    //std::cout <<name.length() << std::endl; 
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

Channel* Database::GetChannel(std::string name)
{
    
    for (SYSTEM_CHANNEL::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        string EXIST(it->first);

    
        if (EXIST.empty() || EXIST.back() != '\0')
            EXIST.push_back('\0');
        // Check if the string contains a null terminator (not needed for std::string)
        /*
        if (name.back() == '\0')
            std::cout << "name contains a null terminator" << std::endl;
        else
            std::cout << "name does not contain a null terminator" << std::endl;
        if (EXIST.back() == '\0')
            std::cout << "EXIST contains a null terminator" << std::endl;
        else
            std::cout << "EXIST does not contain a null terminator" << std::endl;

        name.erase(std::remove(name.begin(), name.end(), '\x05'), name.end());
        name.shrink_to_fit();
        std::cout << "+[" << name[name.length()] << "] | [" << name.length() << "]"<< std::endl;
        std::cout << "-[" << (int)name[name.length() - 1] << "] | [" << name.length() << "]"<< std::endl;
        */
        int x = 0;
        while (x < (int)name.length())
        {
            std::cout << x << " - " << (int)name[x] << std::endl;
            std::cout << x << " - " << name[x] << std::endl;
            x++;
        }
        std::cout << "------------------------------------ " << std::endl;
        x = 0;
        while (x < (int)EXIST.length())
        {
            std::cout << x << " - " << (int)EXIST[x] << std::endl;
            std::cout << x << " - " << EXIST[x] << std::endl;
            x++;
        }

        std::cout << "[" + name + "][" + EXIST +"] # [" << name.length() << "] [" << EXIST.length() << "]" << std::endl;
        if (name == EXIST)
            return channels[EXIST];
    }
	return undefine;
}


