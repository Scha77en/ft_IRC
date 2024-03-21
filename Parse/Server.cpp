#include "Server.hpp"

void extractCommandAndChannel(const char* buffer, std::string& command, std::string& channel) 
{
    std::stringstream ss(buffer);
    ss >> command >> channel;
}

void Server::ChannelCommunication(int NewClientSocket, string channel, string user)
{
    string MSG = Obj.SendMS(channel, user, NewClientSocket);
    
    Obj.DispalyInChannel(NewClientSocket, MSG, informations, channel);
}

void Server::ChannelOperations(int NewClientSocket, const char *xbuffer)
{
    std::string command;
    std::string channel;

    extractCommandAndChannel(xbuffer, command, channel);
    if (command == "JOIN")
    {
        this->informations.UpdateUserChannel(NewClientSocket, channel);

        // bool isRegister = 
        //if (!isRegister)
          //  return ;
        this->Obj.handleJoin(channel, informations.FindUser(NewClientSocket));
        
        this->informations.SetConnection(NewClientSocket, TRUE);

        this->informations.ActiveInChannel(Obj.GetChannelIndex(channel), NewClientSocket);
        //this->Obj.ListUsers(channel, NewClientSocket);
    }
}

bool AcceptOrReject(const char *BUFFER, int ReadByte)
{
    std::string password = "1337";
    if (ReadByte != 5)
        return FALSE;
    for (int i = 0; i < ReadByte - 1; i++)
    {   if (password[i] != BUFFER[i])
            return FALSE;
    }
    return TRUE;
}

void removeNewlines(std::string &str) 
{
    size_t pos = 0;
    while ((pos = str.find('\n', pos)) != std::string::npos)
        str.erase(pos, 1);
}

// Process User Information in Order To Create A DataBase For Client
void Server::LoginInformation(int NewClientSocket)
{
    int ReadByte;
    static int i = 0;
    char BUFFER[BUFFER_SIZE];

    std::stringstream Respond;
    Respond.str("");
    Respond << "ENTER THE USERNAME : " << RESET;
    std::string output = Respond.str();
    send(NewClientSocket, output.c_str(), output.length(), 0);
    ReadByte = recv(NewClientSocket, BUFFER, BUFFER_SIZE, 0);
    //this->BadInput(NewClientSocket, ReadByte);
    BUFFER[ReadByte] = '\0';
    std::string NAME(BUFFER, ReadByte);
    //removeNewlines(NAME);

    this->informations.UserAddInformation(i++, NewClientSocket, NAME);

    Respond.str("");
    Respond << GREEN << "\nWelcome Back " << "@" + NAME << std::endl;
    output = Respond.str();
    send(NewClientSocket, output.c_str(), output.length(), 0);
}

bool Server::ProcessClient()
{
    char BUFFER[BUFFER_SIZE];
    int Big_socketFD, NewClientSocket, ReadByte, sd, Active = 0;

    addrlen = sizeof(server_addr);
    while (TRUE)
    {
        //clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(server_socket, &readfds); 

        // Current highest file descriptor number
        Big_socketFD = server_socket;

        //add child sockets to set 
		for (int i = 0 ; i < MAX_CL ; i++)
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > Big_socketFD) 
				Big_socketFD = sd;
		}

        Active = select( Big_socketFD + 1 , &readfds , NULL , NULL , NULL); 
        
        if (Active < 0)
        {
            std::cerr << "select function error !" << std::endl;
            return EXIT_FAILURE;
        }
        //If something happened on the server_socket, then its an incoming connection 
        if (FD_ISSET(server_socket, &readfds)) 
		{

            if ((NewClientSocket = accept(server_socket, (struct sockaddr *)&server_addr, &addrlen)) == -1) 
            {
                std::cerr << "Error: Accept failed.\n";
                return EXIT_FAILURE;
            }
            std::cout << GREEN << "[*] New connection Detected !" << std::endl;
            std::cout << RESET << "Information => Client Socket : " << NewClientSocket << " IP : " << inet_ntoa(server_addr.sin_addr) << " PORT : " << ntohs(server_addr.sin_port)<< std::endl;
            /*
            std::stringstream Respond;
            Respond.str("");
            Respond << BLUE << "\n[Authentication Required !]\n\n" << RESET << "ENTER THE KEY : ";
            std::string output = Respond.str();
            send(NewClientSocket, output.c_str(), output.length(), 0);
            ReadByte = recv(NewClientSocket, BUFFER, BUFFER_SIZE, 0);
            */
            //if (AcceptOrReject(BUFFER, ReadByte))
            if (true)
            {
                for (int i = 0; i < MAX_CL; i++) 
			    { 
				    if (client_socket[i] == 0) 
				    { 
					    client_socket[i] = NewClientSocket;
					    break; 
				    }
			    }
                LoginInformation(NewClientSocket);
            }
            /*
            else
            {
                Respond.str("");
                Respond << RED << "\n[Authentication Failed !]\n\n" << RESET;
                output = Respond.str();
                send(NewClientSocket, output.c_str(), output.length(), 0);
                close(NewClientSocket);
            }
            */

        }
        for (int i = 0; i < MAX_CL; i++) 
		{ 
			sd = client_socket[i];
            if (informations.IsOnline(sd))
            {
                int Ichannel = informations.InChannelID(sd);
                string channel = Obj.GetChannel(Ichannel);

                string user = informations.FindUser(sd);

                if (FD_ISSET( sd , &readfds))
                {
                    ChannelCommunication(sd, channel, user);

                    Obj.PromptMSG(channel, user, sd);
                }
            }
			else if (FD_ISSET( sd , &readfds)) 
			{
                ReadByte = recv(sd, BUFFER, BUFFER_SIZE, 0);
                std::cout << "Received: " << BUFFER << std::endl;
                BUFFER[ReadByte] = '\0';
                ChannelOperations(sd, BUFFER);
                continue;
			}
		} 
    }
    return EXIT_SUCCESS;
}

bool Server::ServerCreate()
{
    //initialise all client_socket[] to 0 so not checked 
	for (int i = 0; i < MAX_CL; i++) 
		client_socket[i] = 0;
    
    // Create a new socket 
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        std::cerr << "Error: Could not create socket.\n";
        return EXIT_FAILURE;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt));
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
    std::cout << BLUE << "Server listening on port " << PORT << "...\n";
    return this->ProcessClient();
}

Server::Server() 
{
    this->ServerCreate();
}

Server::~Server()
{
    std::cout << RED << "SERVER POWER OFF ....\nDone !" << std::endl;
}
