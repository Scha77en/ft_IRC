#include "Server.hpp"
#include "Database.hpp"
#include <iostream>
#include <sys/socket.h>

Server   *Server::instance = undefine;

void Server::ServerAddr()
{
    std::cout << "[LOCATION] => " << instance << std::endl;
}

void Server::SetPort(int port)
{
    PORT = port;
}

void Server::SetPassword(string password)
{
    PASSWORD = password;
}

int Server::GetPort()
{
    return PORT;
}

string Server::GetPassword()
{
    return PASSWORD;
}

void Server::Setdt(string dt)
{
    this->dt = dt;
}

string Server::Getdt()
{
    return this->dt;
}

Server* Server::getInstance(std::string _PORT, std::string _PASSWORD)
{
    if (instance == undefine) 
    {
        instance = new Server();
        if (std::atoi(_PORT.c_str()) < 1024 || std::atoi(_PORT.c_str()) > 65535)
        {
            std::cerr << "Error: Invalid port number.\n";
            exit(EXIT_FAILURE);
        }
        instance->SetPort(std::atoi(_PORT.c_str()));
        instance->SetPassword(_PASSWORD);
    }
    return instance;
}

void Server::send_reponse(std::string response, int fd)
{
	std::cout << "Response: " << response;
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response send() faild" << std::endl;
}

void Server::WelcomeMsg(int NewClientSocket,string username, string user, string hostname)
{
    (void)user;
    (void)username;
    (void)hostname;
    Client *client = GetClient(NewClientSocket);
    string M001 = ":irc.1337.com 001 "+ client->GetNickname() +" :Welcome to the Internet Relay Network " + client->GetNickname() + "!" + client->GetRealName() + "@"+ client->GetClientIP() + "\r\n";
    string M002 = ":irc.1337.com 002 "+ client->GetNickname() +" :Your host is "+client->GetClientIP()+", running version InspIRCd-3.10\r\n";
    string M003 = ":irc.1337.com 003 "+ client->GetNickname() +" :This server was created on " + this->Getdt() + "\r\n"; 
    string M004 = ":irc.1337.com 004 "+ client->GetNickname() +" irc.1337.com InspIRCd-3.10 iobl\r\n";
    string M005 = ":irc.1337.com 005 "+ client->GetNickname() +" CHANTYPES=# :are supported by this server\r\n";
    send(NewClientSocket, M001.c_str(), M001.length(), 0);
    send(NewClientSocket, M002.c_str(), M002.length(), 0);
    send(NewClientSocket, M003.c_str(), M003.length(), 0);
    send(NewClientSocket, M004.c_str(), M004.length(), 0);
    send(NewClientSocket, M005.c_str(), M005.length(), 0);
}

bool Server::ProcessClient()
{
    // struct pollfd newpoll;
    info = Database::GetInstance();
    while (true)
    {
        int num_ready = poll(&fds[0], fds.size(), -1);
        if (num_ready <= 0)
            throw std::runtime_error("poll failed");
        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds.at(i).revents & POLLIN)
            {
                if (fds[i].fd == server_socket)
                {
                    Client *new_client = new Client();
                    struct sockaddr_in client_addr;
                    struct pollfd newfd;
                    socklen_t addrlen = sizeof(client_addr);
                    int new_client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen);
                    if (new_client_socket == -1)
                    {
                        std::cerr << "Error: Accept failed.\n";
                        return EXIT_FAILURE;
                    }
                    if (fcntl(new_client_socket, F_SETFL, O_NONBLOCK) == -1) 
                    {
                        std::cerr << "Error: Fcntl failed.\n";
                        return EXIT_FAILURE;
                    }
                    newfd.fd = new_client_socket;
                    newfd.events = POLLIN;
                    newfd.revents = 0;

                    new_client->NewClient(new_client_socket);
                    new_client->NewClientIP(client_addr.sin_addr);
                    clients.push_back(new_client);
                    fds.push_back(newfd);
                    std::cout << "Client IP is " << new_client->GetClientIP() << "\n";
                    std::cout << "New client connected " << new_client_socket << " from " << inet_ntoa(client_addr.sin_addr) << "\n";
                }
                else
                {
                    std::vector<std::string> command;
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_received = recv(fds.at(i).fd, buffer, BUFFER_SIZE, 0);
                    if (bytes_received <= 0) 
                    {
                        std::cout << "Client disconnected " << fds.at(i).fd << "\n";
                        // removeClientFromChannel(fds.at(i).fd);
                        close(fds.at(i).fd);
                        removeClient(fds.at(i).fd);
                        removeFDS(fds.at(i).fd);
                    }
                    else
                    {
                        std::cout << "Received: " << buffer << std::endl;
                        info = Database::GetInstance();
                        Client *currClient = GetClient(fds.at(i).fd);
                        currClient->bufferClient = buffer;
                        // size_t pos = currClient->bufferClient.find_first_of("\r\n");
                        // if (pos == std::string::npos)
                        //     continue;
                        command = split_buffer(currClient->bufferClient);
                                std::cout << "Command: " << command[0] << std::endl;
                        for (size_t j = 0; j < command.size(); j++)
                        {
                                checkPass(command.at(j), fds.at(i).fd);
                        }
                        if (!currClient->GetNickname().empty() && !currClient->GetUsername().empty() && !currClient->GetPass().empty() && currClient->GetConnection() == 1 && currClient->GetAuth() == 1)
                        {
                            if (currClient->GetAuth() == 1) {
                                info->AddClient(currClient);
                                WelcomeMsg(fds.at(i).fd, currClient->GetUsername(), currClient->GetNickname(), currClient->GetClientIP());
                            }
                            currClient->SetAuth(0);
                        }
                    }
                }
            }
        }
    }
    close(server_socket);
    return EXIT_SUCCESS;
}

// void Server::removeClientFromChannel(int fd)
// {
//     //remove channel
//     info = Database::GetInstance();
//     // delete client from all channels
//     for (size_t i = 0; i < clients.size(); i++)
//     {
//         if (clients.at(i).GetSocket() == fd)
//         {
//             for (size_t j = 0; j < clients.at(i).GetAllChannels().size(); j++)
//             {
//                 info->HandelMultiPART(clients.at(i).GetAllChannels().at(j), fd);
//             }
//         }
//     }
// }

void Server::removeClient(int fd)
{
    Database *db = Database::GetInstance();
    std::string nickname = db->GetUserBySocket(fd);
    db->RemoveClient(nickname);
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients.at(i)->GetSocket() == fd)
        {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

void Server::removeFDS(int fd)
{
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds.at(i).fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }
}

void Server::closeFDS()
{
    for (size_t i = 0; i < fds.size(); i++)
    {
        std::cout << "Client ==> " << clients.at(i)->GetSocket() << " disconnected.\n";
        close(clients.at(i)->GetSocket());
    }
    if (server_socket != undefine)
        close(server_socket);
}

Client *Server::GetClient(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients.at(i)->GetSocket() == fd)
            return clients.at(i);
    }
    return NULL;
}

std::vector<std::string> Server::split_buffer(string buffer)
{
    std::vector<std::string> command;
    std::string token;
    std::istringstream tokenStream(buffer);
    while (std::getline(tokenStream, token))
    {
        size_t pos = token.find_first_of("\r\n");
        if (pos != std::string::npos)
            token = token.substr(0, pos);
        command.push_back(token);
    }
    return command;
}

void Server::checkPass(std::string &cmd, int NewClientSocket)
{
    if (cmd.empty())
        return;
    std::cout << "[1] -- Command: " << cmd << std::endl;
    Client *client = GetClient(NewClientSocket);
    info = Database::GetInstance();
    std::vector<std::string> command;
    command = spliting_command(cmd);
    size_t pos = cmd.find_first_of("\r\n");
    if (pos != std::string::npos)
        cmd = cmd.substr(pos);
    if ((command.at(0) == "PASS" || command.at(0) == "pass") && command.size())
        authenticate(cmd, NewClientSocket);
    else if ((command.at(0) == "NICK" || command.at(0) == "nick") && command.size())
        Set_nickname(cmd, NewClientSocket);
    else if ((command.at(0) == "USER" || command.at(0) == "user") && command.size())
        Set_username(cmd, NewClientSocket);
    else if (client->GetConnection() == 1 && command.size() && client->GetAuth() == 0)
    {
        std::cout << "[2] -- Command: " << cmd << std::endl;
        if ((command.at(0) == "JOIN" || command.at(0) == "join") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "TOPIC" || command.at(0) == "topic") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "MODE" || command.at(0) == "mode") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "INVITE" || command.at(0) == "invite") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "PART" || command.at(0) == "part") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "LIST" || command.at(0) == "list") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "PRIVMSG" || command.at(0) == "privmsg") && command.size())
        info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "KICK" || command.at(0) == "kick") && command.size())
        info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "QUIT" || command.at(0) == "quit") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "BOT" || command.at(0) == "bot") && command.size())
            info->ParseUserInput(cmd, NewClientSocket);
        else if ((command.at(0) == "PONG" || command.at(0) == "pong") && command.size())
            return ;
        else if (command.size())
            send_reponse(ERR_UNKNOWNCOMMAND(command.at(0)), NewClientSocket);
    }
    else if (command.size())
        send_reponse(ERR_UNKNOWNCOMMAND(command.at(0)), NewClientSocket);
}

void Server::Set_username(std::string &cmd, int NewClientSocket)
{
    std::stringstream ss;
    std::string username;
    std::string hostname;
    std::string modestate;
    std::string realname;

    Client *client = GetClient(NewClientSocket);
    if (client->GetConnection() == 0)
    {
        send_reponse(ERR_NOTREGISTERED(client->GetNickname()), NewClientSocket);
        return;
    }
    cmd = cmd.substr(4); // Remove USER
    ss << cmd;
    ss >> username >> modestate >> hostname;
    std::getline(ss, realname);
    if (username.empty() || modestate.empty() || hostname.empty() || realname.find_first_not_of("\t\v ") == std::string::npos)
    {
        send_reponse(ERR_NOTENOUGHPARAM(client->GetNickname()), NewClientSocket);
        return;
    }
    std::cout << "[11111111] ------ [" << realname << "]" << std::endl;
    int pos = realname.find_first_not_of("\t\v ");
    if (realname[pos] == ':') {
        std::cout << "============= [1]" << std::endl;
        realname = realname.substr(realname.find_first_not_of("\t\v ") + 1);
    }
    else {
        std::cout << "============= [2]" << std::endl;
        std::stringstream sasa;
        sasa << realname;
        realname = "";
        sasa >> realname;
    }
    std::cout << "[0000000] ------ [" << realname << "]" << std::endl;
    std::cout << "Client username = " << client->GetUsername() << std::endl;
    if (client->GetUsername().empty())
    {
        client->SetUserName(username);
        client->SetRealName(realname);
        client->SetAuth(1);
        std::cout << "client username = " << client->GetUsername() << std::endl;
        std::cout << "client realname = " << client->GetRealName() << std::endl;
    }
    else
        send_reponse(ERR_ALREADYREGISTERED(client->GetNickname()), NewClientSocket);
}

// void Server::Set_username(std::string &cmd, int NewClientSocket)
// {
//     Client *client = GetClient(NewClientSocket);
//     if (client->GetConnection() == 0)
//     {
//         send_reponse(ERR_NOTREGISTERED(std::string ("guess")), NewClientSocket);
//         return;
//     }
//     cmd = cmd.substr(4); // Remove USER
//     size_t pos = cmd.find_first_not_of("\t\v ");
//     if (pos < cmd.size())
//     {
//         cmd = cmd.substr(pos);
//         if (cmd[0] == ':')
//             cmd.erase(cmd.begin());
//     }
//     if (pos == std::string::npos || cmd.empty())
//     {
//         send_reponse(ERR_NOTENOUGHPARAM(std::string ("guess")), NewClientSocket);
//         return;
//     }
//     else if (client->GetUsername().empty())
//     {
//         std::cout << "Username is set to " << cmd << ".\n";
//         client->setUsername(cmd);
//         client->SetAuth(1);
//     }
//     else
//         send_reponse(ERR_ALREADYREGISTERED(client->GetNickname()), NewClientSocket);
// }

void Server::Set_nickname(std::string &cmd, int NewClientSocket)
{
    Client *client = GetClient(NewClientSocket);
    if (client->GetConnection() == 0) // Check if the client is connected
    {
        send_reponse(ERR_NOTREGISTERED(std::string ("guess")), NewClientSocket);
        return;
    }
    cmd = cmd.substr(4); // Remove NICK
    size_t pos = cmd.find_first_not_of("\t\v ");
    if (pos < cmd.size())
    {
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if (pos == std::string::npos || cmd.empty()) // Check if the nickname is empty
    {
        send_reponse(ERR_NOTENOUGHPARAM(std::string ("guess")), NewClientSocket);
        return;
    }
    else if (client->GetNickname().empty()) // Check if the nickname is already set
    {
        if (checkNickName(cmd))
        {
            send_reponse(ERR_NICKINUSE(cmd), NewClientSocket);
            return;
        }
        std::cout << "Nickname is set to " << cmd << ".\n";
        client->SetNickName(cmd);
        client->SetAuth(1);
    }
    else
        send_reponse(ERR_ERRONEUSNICK(cmd), NewClientSocket);
}

bool Server::checkNickName(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (clients.at(i)->GetNickname() == nickname)
			return true;
	}
	return false;
}

void Server::authenticate(std::string &cmd, int NewClientSocket)
{
    Client *client = GetClient(NewClientSocket);

    cmd = cmd.substr(4); // Remove PASS
    size_t pos = cmd.find_first_not_of("\t\v ");
    if (pos < cmd.size())
    {
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if (pos == std::string::npos || cmd.empty())
        send_reponse(ERR_NOTREGISTERED(std::string ("guess")), NewClientSocket);
    else if (client->GetConnection() == 0)
    {
        string pass = cmd;
        if (pass == PASSWORD)
        {
            std::cout << "Password is correct.\n";
            // send_reponse(ERR_PASSCORRECT(string(" # ")), NewClientSocket);
            client->SetPass(cmd);
            client->SetConnection(1);
            client->SetAuth(1);
        }
        else
            send_reponse(ERR_INCORPASS(client->GetNickname()), NewClientSocket);
    }
    else
        send_reponse(ERR_ALREADYREGISTERED(client->GetNickname()), NewClientSocket);

}

std::vector<std::string> Server::spliting_command(string &command)
{
    std::vector<std::string> command_;
    std::string token;
    std::istringstream tokenStream(command);
    while (tokenStream >> token)
    {
        command_.push_back(token);
        token.clear();
    }
    return command_;
}

bool Server::ServerCreate()
{
    time_t now = time(0);
    this->Setdt(ctime(&now));

    // Create a new socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        std::cerr << "Error: Could not create socket.\n";
        return EXIT_FAILURE;
    }
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) == EPIPE)
    {
        std::cerr << "Error: Setsockopt failed.\n";
        return EXIT_FAILURE;
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Error: Setsockopt failed.\n";
        return EXIT_FAILURE;
    }

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error: Fcntl failed.\n";
        return EXIT_FAILURE;
    }

    //type of socket created 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	server_addr.sin_port = htons(PORT);

	//bind the socket to localhost port 8080 
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
    {
        std::cerr << "Error: Bind failed.\n";
        return EXIT_FAILURE;
    }
    // Prepare to accept connections on socket FD.
	if (listen(server_socket, 5) == -1) 
    {
        std::cerr << "Error: Listen failed.\n";
        return EXIT_FAILURE;
    }
    addrlen = sizeof(server_addr);
    std::cout << BLUE << "Server listening on port " << PORT << "...\n" << RESET;
    struct pollfd newfd;
    newfd.fd = server_socket;
    newfd.events = POLLIN;
    newfd.revents = 0;
    fds.push_back(newfd);

    return this->ProcessClient();
}
