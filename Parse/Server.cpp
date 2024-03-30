#include "Server.hpp"

Server   *Server::instance = undefine;

void Server::ServerAddr()
{
    std::cout << "[LOCATION] => " << instance << std::endl;
}

Server* Server::getInstance()
{
    if (instance == undefine) 
        instance = new Server();
    return instance;
}

void Extract(const char* buffer, std::string& Value, string &command) 
{
    std::stringstream ss(buffer);
    ss >> command >> Value;
}

bool Server::GetDataInformation(int NewClientSocket, string &command, string &output)
{
    string CHECK = "";
    char BUFFER[BUFFER_SIZE];

    int bytes_received = recv(NewClientSocket, BUFFER, sizeof(BUFFER), 0);
    if (bytes_received <= 0)
        return undefine;
    BUFFER[bytes_received] = '\0';
    string Received(BUFFER);
    RemoveNewLines(Received);
    Extract(BUFFER, Received, CHECK);
    char* charPtr = const_cast<char*>(CHECK.c_str());
    for (size_t i = 0; i < strlen(charPtr); ++i)
        charPtr[i] = std::toupper(charPtr[i]);
    string UpperCheck(charPtr);
    //std::cout << "command => [" + UpperCheck + "]" << std::endl;
    if (command != UpperCheck || Received.empty())
        return undefine;
    //std::cout << "Value => [" + Received + "]" << std::endl;
    output = Received;
    return (1);
}

void WelcomeMsg(int NewClientSocket,string username, string user, string hostname)
{
    string M001 = ":irc.1337.com 001 "+username+" :Welcome to the Internet Relay Network "+username+"!"+user+"@"+hostname+"\n";
    string M002 = ":irc.1337.com 002 "+username+" :Your host is "+hostname+", running version InspIRCd-3.10\n";
    string M003 = ":irc.1337.com 003 "+username+" :This server was created on " + "<date>" + "\n"; 
    string M004 = ":irc.1337.com 004 "+username+" irc.1337.com InspIRCd-3.10 iobl\n";
    string M005 = ":irc.1337.com 005 "+username+" CHANTYPES=# :are supported by this server\n";
    send(NewClientSocket, M001.c_str(), M001.length(), 0);
    send(NewClientSocket, M002.c_str(), M002.length(), 0);
    send(NewClientSocket, M003.c_str(), M003.length(), 0);
    send(NewClientSocket, M004.c_str(), M004.length(), 0);
    send(NewClientSocket, M005.c_str(), M005.length(), 0);
}

bool Server::ProccessUserData(int NewClientSocket, struct in_addr ClientIP)
{
    bool is_log = 1;
    string command = "";

    info = Database::GetInstance();

    command = "PASS";
    string password = "";
    if (!GetDataInformation(NewClientSocket, command, password))
        is_log = undefine;
    if (is_log == undefine)
        return (undefine);
    command = "NICK";
    string username = "";
    if (!GetDataInformation(NewClientSocket, command, username))
        is_log = undefine;
    if (is_log == undefine)
        return (undefine);
    command = "USER";
    string name = "";
    if (!GetDataInformation(NewClientSocket, command, name))
        is_log = undefine;
    if (is_log == undefine)
        return (undefine);
    info->AddClient(username);
    Client * NewClient = info->GetClient(username);
    NewClient->SetName(name);
    NewClient->SetConnection(1);
    NewClient->SetPass(password);
    NewClient->NewClient(NewClientSocket);
    NewClient->NewClientIP(ClientIP);
    string hostname(inet_ntoa(ClientIP));
    WelcomeMsg(NewClientSocket,username, name, hostname);
    return TRUE;
}

/*
std::string  welcome = ":ayman.irc.chat 001 c1 :Welcome to the irc.Chat Internet Relay Chat Network c1\r\n";

    std::cout << "-->" << welcome << std::endl;

*/

/*
nc localhost 5554 (Copy and past)
PASS 369
NICK atye3
USER tyue
*/

void Server::StartSession()
{
    char BUFFER[BUFFER_SIZE];

    for (int i = 1; i < MAX_CLIENTS; ++i)
    {
        if (fds[i].fd != undefine && (fds[i].revents & POLLIN)) 
        {
            info = Database::GetInstance();
            if (info->GetUserBySocket(fds[i].fd).empty())
                ProccessUserData(fds[i].fd, client_ips[i]);
            else
            {
                int bytes_received = recv(fds[i].fd, BUFFER, BUFFER_SIZE - 1, 0);
                if (bytes_received > 0)
                {
                    BUFFER[bytes_received] = '\0';
                    string Received(BUFFER);
                    RemoveNewLines(Received);
                    int x = 0;
                    while (Received[x])
                    {
                        if ((int)Received[x] == 13)
                        {
                            Received[x] = '\0';
                            break;
                        }
                        x++;
                    }
                    std::cout <<  "# Received : [" + Received + "]" << std::endl;
                    info->ParseUserInput(Received, fds[i].fd);
                }

            }
            /*
            else if (bytes_received == 0) 
            {
                close(fds[i].fd);
                fds[i].fd = undefine;
            } 
            else 
            {
                close(fds[i].fd);
                fds[i].fd = undefine;
            }
            */
        }
    }
}

bool Server::ProcessClient() 
{
    while (true) 
    {
        int num_ready = poll(fds, MAX_CLIENTS, -1);
        if (num_ready <= 0)
            continue;
        if (fds[0].revents & POLLIN) 
        {
            struct sockaddr_in client_addr;
            socklen_t CL_addrlen = sizeof(client_addr);
            int NewClientSocket = accept(server_socket, (struct sockaddr *)&client_addr, &CL_addrlen);
            if (NewClientSocket == -1)
                continue;
            
            int empty_slot = -1;
            for (int i = 1; i < MAX_CLIENTS; ++i) 
            {
                if (fds[i].fd == undefine) 
                {
                    empty_slot = i;
                    fds[i].fd = NewClientSocket;
                    client_ips[i] = client_addr.sin_addr;
                    break;
                }
            }
            if (empty_slot == -1)
            {
                close(NewClientSocket);
                continue;
            }
        }
        this->StartSession();
    }
    return EXIT_SUCCESS;
}


void Server::RemoveNewLines(string &str) 
{
    size_t pos = 0;
    while ((pos = str.find('\n', pos)) != std::string::npos)
        str.erase(pos, 1);
    while ((pos = str.find('\r', pos)) != std::string::npos)
        str.erase(pos, 1);
}

bool Server::ServerCreate()
{   
    // Create a new socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        std::cerr << "Error: Could not create socket.\n";
        return EXIT_FAILURE;
    }
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt));
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
    //fcntl(server_socket, F_SETFL, O_NONBLOCK);
    std::cout << BLUE << "Server listening on port " << PORT << "...\n" << RESET;
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;
    for (int i = 1; i < MAX_CLIENTS; ++i) 
    {
        fds[i].fd = undefine;
        fds[i].events = POLLIN;
    }
    return this->ProcessClient();
}
