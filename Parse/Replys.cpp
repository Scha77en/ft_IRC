#include "Database.hpp"

void ERR_BADCHANNELKEY_475(string name, string username, int UserSocket, string IP)
{
    string output = ":irc.1337.com" + IP + " 475 " + username + " #" + name + " :Cannot join channel (+k) - bad key\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CHANNELISFULL_471(string name, int UserSocket, string username, string IP)
{
    string output = ":irc.1337.com" + IP + " 471 " + username + " #" + name + " :Cannot join channel (+l)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_BANNEDFROMCHAN_474(string name, int UserSocket, string username, string IP)
{
    string output = ":irc.1337.com" + IP + " 474 " + username + " #" + name + " :Cannot join channel (+b)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_INVITEONLYCHAN_473(string name, int UserSocket, string username, string IP)
{
    string output = ":irc.1337.com" + IP + " 473 " + username + " #" + name + " :Cannot join channel (+i)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_UNKNOWNCOMMAND_421(string command, int UserSocket, string username)
{
    string output = ":irc.1337.com 421 " + username + " " + command + " :Unknown command\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_TOOMANYCHANNELS_405(string name, int UserSocket, string username)
{
    string output = ":irc.1337.com 405 " + username + " #" + name + " :You have joined too many channels\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NEEDMOREPARAMS_461(string name, int UserSocket, string username)
{
    string output = ":irc.1337.com 461 " + username + " " + name + " :Not enough parameters\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOTONCHANNEL_442(string username, int UserSocket, string ChannelName)
{
    string output = ":irc.1337.com 442 " + username + " " + ChannelName + " :You're not on that channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CHANOPRIVSNEEDED_482(string username, int UserSocket, string ChannelName)
{
    string output = ":irc.1337.com 482 " + username + " " + ChannelName + " :You're not a channel operator\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NORECIPIENT_411(string username, int UserSocket)
{
    string output = ":irc.1337.com 411 " + username + " :No recipient given PRIVMSG\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHCHANNEL_403(string username, string target, int UserSocket)
{
    string output = ":irc.1337.com 403 " + username + " " + target + " :No such channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CANNOTSENDTOCHAN_404(string username, string target, int UserSocket)
{
    string output = ":irc.1337.com 403 " + username + " " + target + " :Cannot send to channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHNICK_401(string username, string target, int UserSocket)
{
    string output = ":irc.1337.com 401 " + username + " " + target + " :No such nick/channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOTEXTTOSEND_412(string username, int UserSocket)
{
    string output = ":irc.1337.com 412 " + username + " :No text to send\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void RPL_AWAY_301(string username, string target, string msg ,int UserSocket)
{
    string output = ":irc.1337.com 301 " + username + " " + target + " " + msg + "\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}