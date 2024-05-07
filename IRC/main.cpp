#include "../Headers/Server.hpp"
#include "../Headers/Channel.hpp"
#include <iostream>
#include <memory>
#include <sys/signal.h>

// void leaks()
// {
// 	char cmd[1024];
// 	sprintf(cmd, "lsof -p %d", getpid());
// 	system(cmd);
// }

void leaks()
{
	system("leaks ircserv");
}

void	handle_signal(int signum){
	
	(void)signum;
	Database *db = Database::GetInstance();
	db->CloseServer();
}

int main(int ac, char **av)
{
	atexit(leaks);
	if (ac != 3)
	{
		std::cerr << "Usage: ./parse [PORT] [PASSWORD]" << std::endl;
		return 1;
	}
	else if (av[0] == NULL || av[1] == NULL || av[2] == NULL || av[2][0] == '\0')
	{
		std::cerr << "Usage: ./parse [PORT] [PASSWORD]" << std::endl;
		return 1;
	}
	std::string pass = av[2];
	if (pass.find_first_not_of(" \r\t") == std::string::npos)
	{
		std::cerr << "Usage: ./parse [PORT] [PASSWORD]" << std::endl;
		return 1;
	}
	try
	{
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
		Server *server = Server::getInstance(av[1], av[2]);
		server->ServerCreate();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
