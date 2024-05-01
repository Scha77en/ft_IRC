#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <ctime>

#include "Channel.hpp"
#include "Database.hpp"

#define undefine 0
#define TRUE 1
#define FALSE 0

//error reply
#define ERR_INCORPASS(nickname) (": 464 " + nickname + " :Password incorrect !" + "\r\n" )
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + "\r\n" )
#define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + "\r\n" )
#define ERR_NICKINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + "\r\n")
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + "\r\n")
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + "\r\n")
#define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + "\r\n")
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + "\r\n")
#define ERR_UNKNOWNCOMMAND(nickname) (": 421 " + nickname + " :Unknown command" + "\r\n")


const int BUFFER_SIZE = 1024;


class Server 
{
    private:

        Server() {}

        static Server* instance;

        struct sockaddr_in server_addr;
        std::vector<struct pollfd> fds;
        std::vector<struct in_addr> client_ips;
        std::vector<Client *> clients;


        socklen_t addrlen;
        int server_socket;
        int PORT;
        std::string PASSWORD;
        std::string dt;

        Database *info;

    public:

        void Setdt(std::string dt);
        std::string Getdt();
        void SetPort(int port);
        void SetPassword(std::string password);
        int  GetPort();
        std::string  GetPassword();

        Client *GetClient(int fd);
        std::vector<std::string> split_buffer(std::string buffer);
        std::vector<std::string> spliting_command(std::string &command);
        
        void WelcomeMsg(int NewClientSocket, std::string username, std::string user,std::string hostname);
        void removeClient(int fd);
        void removeFDS(int fd);
        bool checkNickName(std::string &command);
        void send_reponse(std::string response, int fd);
        void authenticate(std::string &cmd, int NewClientSocket);
        void Set_nickname(std::string &cmd, int NewClientSocket);
        void Set_username(std::string &cmd, int NewClientSocket);
        void checkPass(std::string &cmd, int NewClientSocket);
        bool ServerCreate();
        bool ProcessClient();
        void closeFDS();
        static Server* getInstance(std::string _PORT, std::string _PASSWORD);
};

#endif
