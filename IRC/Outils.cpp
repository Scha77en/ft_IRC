#include "../Headers/Database.hpp"

void ExtractTwoRnages(std::string data, std::string &ch, std::string &ky) 
{
    std::stringstream Process(data);

    Process >> ch >> ky;
}

void RemoveCharacter(int ASCII, std::string &target)
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

void GetCommand(const char* buffer, std::string& command, std::string& channel) 
{
    std::stringstream ss(buffer);
    ss >> command >> channel;
}

void RemoveNewLine(std::string &str)
{
    size_t pos = 0;
    while ((pos = str.find('\n', pos)) != std::string::npos)
        str.erase(pos, 1);
}

STORE TokenRangeExtract(std::string data)
{
    STORE Extracted;

    std::string MiniToken;
    std::istringstream Object(data);

    while (std::getline(Object, MiniToken, ','))
        Extracted.push_back(MiniToken);
    return (Extracted);
}

STORE GetSendingList(std::string data, std::string &message)
{
    STORE List;

    std::string Ranges;
    std::string token;
    std::stringstream Obj(data);
    
    std::getline(Obj, Ranges, ' ');
    std::getline(Obj, message);
    
    std::stringstream Object(Ranges);
    while (std::getline(Object, token, ','))
    {
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        List.push_back(token);
    }
    return (List);
}

SYSTEM_KEYVAL parseChannels(std::string &input, int SK, std::string username) 
{
    SYSTEM_KEYVAL result;
    std::string ch, ky;
    STORE RangeChannels, RangeKeys;

    ExtractTwoRnages(input, ch, ky);

    RangeChannels = TokenRangeExtract(ch);
    RangeKeys = TokenRangeExtract(ky);

    for (size_t i = 0; i < RangeChannels.size(); ++i) 
    {
        if (i < RangeKeys.size())
        {
            if (RangeChannels[i][0] == '#')
            {
                result.push_back(std::make_pair(RangeChannels[i], RangeKeys[i]));
            }
            else
            {
                ERR_NOSUCHCHANNEL_403(username, RangeChannels[i], SK);
                return (result);
            }
        }
        else
        {
            if (RangeChannels[i][0] == '#')
                result.push_back(std::make_pair(RangeChannels[i], ""));
            else
            {
                ERR_NOSUCHCHANNEL_403(username, RangeChannels[i], SK);
                return (result);
            }
        }
    }
    return result;
}

bool Protection(std::string data)
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

void CleanInput(std::string &data, char Delimiter)
{
    (void)Delimiter;

    std::string arg;
    std::string output;
    std::string command;

    std::stringstream Object(data);
    
    data = "";

    Object >> command >> arg;
    if (command.empty())
        return ;
    if (arg.empty())
    {
        data += command;
        return ;
    }
    std::getline(Object, data);
    if (data.empty())
    {
        data = command + " " + arg;
        return;
    }
    output = command + " " + arg + data;
    data = output;
}

bool Potection403(std::string data)
{
    size_t position = data.find('#');
    return (position == std::string::npos);
}