#include "../headers/server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "You must include PORT and PASSWORD to proceed." << std::endl;
		return (1);
	}

	signal(SIGPIPE, SIG_IGN);

	Server server;

	try
	{
		server.parseArgs(argv[1], argv[2]);
		server.setupListener();
		server.awaitingTraffic();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
