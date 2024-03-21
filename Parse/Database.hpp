#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>

#define TRUE 1
#define FALSE 0

#define MAX_CL 30
#define MAX_CHANNEL 20

typedef std::string string;

struct Client_Structure
{
    string name;
    string ChannelID[MAX_CHANNEL];

    int id;
    int socket;
    int CurrentChannel;
    int Permission[MAX_CHANNEL];

    bool online;
};

class DataBase
{
    public:
        DataBase();
        ~DataBase();

        bool IsOnline(int NewClientSocket);
    
        int UserSocket(string username);
        int UserIndex(int NewClientSocket);
        int InChannelID(int NewClientSocket);

        string FindUser(int NewClientSocket);

        void ActiveInChannel(int index, int NewClientSocket);
        void SetConnection(int NewClientSocket, bool status);
        void UserAddInformation(int ID, int Socket, string Name);

        bool UpdateUserChannel(int NewClientSocket, string &channel);
    
    private:
        Client_Structure users[MAX_CL];
};

#endif