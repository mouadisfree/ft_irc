#include "../../include/server.hpp"

void	Server::runServerEventLoop()
{
	fd_set totalfds, readfds, writefds;

	FD_ZERO(&totalfds);

	FD_SET(server_fd, &totalfds);

	while(!g_shutdownRequested) {

		readfds = totalfds;

		FD_ZERO(&writefds);
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (!it->second.outBuffer.empty())
				FD_SET(it->first, &writefds);
		}

		int	res = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);

		if (res < 0)
		{
			if (errno == EINTR) continue ;
			throw std::runtime_error("Systemcall `select()` failed.");
		}

		if (!res) continue ;

		if (FD_ISSET(server_fd, &readfds)) acceptNewClientConnection(this->clients, totalfds, server_fd);

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step != C_CLOSE_CONNECTION && FD_ISSET(it->first, &readfds))
				handleReadRequest(it->second);
		}

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step != C_CLOSE_CONNECTION && !it->second.outBuffer.empty())
				flushClient(it->second);
		}

		std::vector<int> clientsReadyToBeRemoved;

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step == C_CLOSE_CONNECTION)
				clientsReadyToBeRemoved.push_back(it->first);
		}

		disconnectAndRemoveClosedClients(clientsReadyToBeRemoved, totalfds);
	}

	shutdownServer();
}

void	Server::shutdownServer()
{
	std::vector<int>	remaining;

	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		remaining.push_back(it->first);

	fd_set	dummy;
	FD_ZERO(&dummy);
	disconnectAndRemoveClosedClients(remaining, dummy);

	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		delete it->second;
	channels.clear();

	close(server_fd);

	std::cout << "\nirc server shutting down..." << std::endl;
}
