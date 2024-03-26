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

void ExtractUserName(const char* buffer, std::string& UserName) 
{
    std::stringstream ss(buffer);
    ss >> UserName;
}

bool Server::ProccessUserData(int NewClientSocket)
{
    char BUFFER[BUFFER_SIZE];

    info = Database::GetInstance();
    string ProcessInfo = "ENTER USERNAME : ";
    send(NewClientSocket, ProcessInfo.c_str(), ProcessInfo.length(), 0);

    int bytes_received = recv(NewClientSocket, BUFFER, BUFFER_SIZE, 0);
    BUFFER[bytes_received] = '\0';
    string Received(BUFFER);
    RemoveNewLines(Received);
    ExtractUserName(BUFFER, Received);

    std::stringstream Respond;

    Respond << GREEN << "\nWelcome Back " << "@" + Received << RESET << std::endl;
    string output = Respond.str();
    send(NewClientSocket, output.c_str(), output.length(), 0);

    info->AddClient(Received);
    Client * NewClient = info->GetClient(Received);
    NewClient->NewClient(NewClientSocket);

    return TRUE;
}

void Server::StartSession()
{
    char BUFFER[BUFFER_SIZE];

    for (int i = 1; i < MAX_CLIENTS; ++i)
    {
        if (fds[i].fd != undefine && (fds[i].revents & POLLIN)) 
        {
            
            int bytes_received = recv(fds[i].fd, BUFFER, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0)
            {
                BUFFER[bytes_received] = '\0';
                string Received(BUFFER);
                RemoveNewLines(Received);
                std::cout << BLUE << "* Received : " << RESET << "[" << Received << "]" << std::endl;
                info = Database::GetInstance();
                info->ParseUserInput(Received, fds[i].fd);
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
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    for (int i = 1; i < MAX_CLIENTS; ++i) 
    {
        fds[i].fd = undefine;
        fds[i].events = POLLIN;
    }

    while (true) 
    {
        int num_ready = poll(fds, MAX_CLIENTS, -1);
        if (num_ready <= 0)
            continue;

        if (fds[0].revents & POLLIN) 
        {
            int NewClientSocket = accept(server_socket, (struct sockaddr *)&server_addr, &addrlen);
            if (NewClientSocket == -1)
                continue;
            
            int empty_slot = -1;
            for (int i = 1; i < MAX_CLIENTS; ++i) 
            {
                if (fds[i].fd == undefine) 
                {
                    fds[i].fd = NewClientSocket;
                    empty_slot = i;
                    break;
                }
            }
            if (empty_slot == -1)
            {
                close(NewClientSocket);
                continue;
            }
            this->ProccessUserData(NewClientSocket);
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
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
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
    std::cout << BLUE << "Server listening on port " << PORT << "...\n";
    return this->ProcessClient();
}
