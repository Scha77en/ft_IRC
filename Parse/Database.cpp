#include "Database.hpp"

DataBase::DataBase() {}

DataBase::~DataBase() {}

int DataBase::UserIndex(int NewClientSocket)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].socket == NewClientSocket)
            return (i);
    }
    return (-1);
}

int DataBase::UserSocket(string username)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].name == username)
            return (users[i].socket);
    }
    return (-1);
}

bool DataBase::IsOnline(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].online);
}

void DataBase::SetConnection(int NewClientSocket, bool status)
{
    users[UserIndex(NewClientSocket)].online = status;
}

std::string DataBase::FindUser(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].name);
}

bool DataBase::UpdateUserChannel(int NewClientSocket, string &channel)
{
    for (int i = 0; i < MAX_CL; i++)
    {
        if (users[i].socket == NewClientSocket)
        {
            int ChannelIndex = 0;
            for (int x = 0; x < MAX_CHANNEL; x++)
            {
                if (users[i].ChannelID[x].empty())
                    ChannelIndex = x;
                
                if (users[i].ChannelID[x] == channel)
                    return (FALSE);
            }
            users[i].ChannelID[ChannelIndex] = channel;
            return (TRUE);
        }
    }
    return (TRUE);
}

void DataBase::ActiveInChannel(int index, int NewClientSocket)
{
    users[UserIndex(NewClientSocket)].CurrentChannel = index;
}

int DataBase::InChannelID(int NewClientSocket)
{
    return (users[UserIndex(NewClientSocket)].CurrentChannel);
}

void DataBase::UserAddInformation(int ID, int Socket, string Name)
{
    users[ID].id = ID;
    users[ID].name = Name;
    users[ID].socket = Socket;
    for (size_t i = 0; i < MAX_CHANNEL; i++)
    {
        users[ID].ChannelID[i] = "";
        users[ID].Permission[i] = 0;
    }
    users[ID].online = FALSE;
    users[ID].CurrentChannel = 0;
}