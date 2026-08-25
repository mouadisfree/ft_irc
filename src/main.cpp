#include "../include/server.hpp"

volatile sig_atomic_t g_shutdownRequested = 0;

void	handleShutdownSignal(int signum)
{
	(void)signum;
	g_shutdownRequested = 1;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "You must include PORT and PASSWORD to proceed." << std::endl;
		return (1);
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, handleShutdownSignal);
	signal(SIGQUIT, handleShutdownSignal);

	Server server;

	try
	{
		server.parseCommandLineArguments(argv[1], argv[2]);
		server.setupServerSocketAndStartListening();
		server.runServerEventLoop();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
