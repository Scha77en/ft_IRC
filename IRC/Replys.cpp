#include "../Headers/Database.hpp"

void ERR_BADCHANNELKEY_475(std::string name, std::string username, int UserSocket, std::string IP)
{
    std::string output = ":irc.1337.com" + IP + " 475 " + username + " " + name + " :Cannot join channel (+k) - bad key\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CHANNELISFULL_471(std::string name, int UserSocket, std::string username, std::string IP)
{
    std::string output = ":irc.1337.com" + IP + " 471 " + username + " " + name + " :Cannot join channel (+l)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_BANNEDFROMCHAN_474(std::string name, int UserSocket, std::string username, std::string IP)
{
    std::string output = ":irc.1337.com" + IP + " 474 " + username + " " + name + " :Cannot join channel (+b)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_INVITEONLYCHAN_473(std::string name, int UserSocket, std::string username, std::string IP)
{
    std::string output = ":irc.1337.com" + IP + " 473 " + username + " " + name + " :Cannot join channel (+i)\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_UNKNOWNCOMMAND_421(std::string command, int UserSocket, std::string username)
{
    std::string output = ":irc.1337.com 421 " + username + " " + command + " :Unknown command\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_TOOMANYCHANNELS_405(std::string name, int UserSocket, std::string username)
{
    std::string output = ":irc.1337.com 405 " + username + " " + name + " :You have joined too many channels\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NEEDMOREPARAMS_461(std::string name, int UserSocket, std::string username)
{
    std::string output = ":irc.1337.com 461 " + username + " " + name + " :Not enough parameters\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOTONCHANNEL_442(std::string username, int UserSocket, std::string ChannelName)
{
    std::string output = ":irc.1337.com 442 " + username + " " + ChannelName + " :You're not on that channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CHANOPRIVSNEEDED_482(std::string username, int UserSocket, std::string ChannelName)
{
    std::string output = ":irc.1337.com 482 " + username + " " + ChannelName + " :You're not a channel operator\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NORECIPIENT_411(std::string username, int UserSocket)
{
    std::string output = ":irc.1337.com 411 " + username + " :No recipient given PRIVMSG\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHCHANNEL_403(std::string username, std::string target, int UserSocket)
{
    std::string output = ":irc.1337.com 403 " + username + " " + target + " :No such channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_CANNOTSENDTOCHAN_404(std::string username, std::string target, int UserSocket)
{
    std::string output = ":irc.1337.com 403 " + username + " " + target + " :Cannot send to channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOSUCHNICK_401(std::string username, std::string target, int UserSocket)
{
    std::string output = ":irc.1337.com 401 " + username + " " + target + " :No such nick/channel\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void ERR_NOTEXTTOSEND_412(std::string username, int UserSocket)
{
    std::string output = ":irc.1337.com 412 " + username + " :No text to send\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}

void RPL_AWAY_301(std::string username, std::string target, std::string msg ,int UserSocket)
{
    std::string output = ":irc.1337.com 301 " + username + " " + target + " " + msg + "\n";
    send(UserSocket, output.c_str(), output.length(), 0);
}


// --------- ERRORS AND REPLYS ------------

void    Database::ERR_442_NOTONCHANNEL(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_NOTONCHANNEL(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_461_NEEDMOREPARAMS(std::string username, std::string command, int UserSocket)
{
	std::string error = ERR_NEEDMOREPARAMS(username, command);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_403_NOSUCHCHANNEL(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_NOSUCHCHANNEL(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::ERR_482_CHANOPRIVSNEEDED(std::string username, std::string channelName, int UserSocket)
{
	std::string error = ERR_CHANOPRIVSNEEDED(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void    Database::RPL_NOTOPIC_331(std::string username, std::string channelName, int UserSocket)
{
	std::string error = RPL_NOTOPIC(username, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}
void    Database::RPL_TOPIC_332(std::string username, std::string channelName, std::string topic, int UserSocket)
{
	std::string error = RPL_TOPIC(username, channelName, topic);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::RPL_324_CHANNELMODEIS(std::string username, std::string channelName, int UserSocket)
{
	std::string modes = channels[channelName]->GetModes();
	std::string error = RPL_CHANNELMODEIS(username, channelName, modes);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_472_UNKNOWNMODE(std::string username, char mode, int UserSocket)
{
	std::string error = ERR_UNKNOWNMODE(username, mode);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_401_NOSUCHNICK(std::string username, std::string target, int UserSocket)
{
	std::string error = ERR_NOSUCHNICK_(username, target);
	send(UserSocket, error.c_str(), error.length(), 0);
}


void	Database::ERR_441_USERNOTINCHANNEL(std::string UserName, std::string target, std::string channelName, int UserSocket)
{
	std::string error = ERR_USERNOTINCHANNEL(UserName, target, channelName);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::RPL_341_INVITING(std::string UserName, std::string target, std::string channel, int UserSocket)
{
	Channel *channel_ = channels[channel];
	channel_->AddInvited(target);
	std::string error = RPL_INVITING(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}

void	Database::ERR_443_USERONCHANNEL(std::string UserName, std::string target, std::string channel, int UserSocket)
{
	std::string error = ERR_USERONCHANNEL(UserName, target, channel);
	send(UserSocket, error.c_str(), error.length(), 0);
}
