#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>

#define MAX_CL 30

typedef std::string string;

struct Client_Structure
{

    string name;

    int id;
    int socket;
    int permission;

};

class DataBase
{
    public:
        DataBase();
        ~DataBase();

        std::string FindUser(int NewClientSocket);

        void UserAddInformation(int ID,int Permission, int Socket, string Name);
    
    private:
        Client_Structure users[MAX_CL];
};

#endif