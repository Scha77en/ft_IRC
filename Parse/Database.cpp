#include "Database.hpp"

DataBase::DataBase() {}

DataBase::~DataBase() {}

std::string DataBase::FindUser(int NewClientSocket)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].socket == NewClientSocket)
            return (users[i].name);
    }
    return ("");
}

void DataBase::UserAddInformation(int ID,int Permission, int Socket, string Name)
{
    users[ID].id = ID;
    users[ID].name = Name;
    users[ID].socket = Socket;
    users[ID].permission = Permission;
}