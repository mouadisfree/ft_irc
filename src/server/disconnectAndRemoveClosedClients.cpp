#include "../../include/server.hpp"

void	Server::disconnectAndRemoveClosedClients(std::vector<int> clientsToBeRemoved, fd_set &totalfds)
{
	for (std::vector<int>::iterator it = clientsToBeRemoved.begin(); it != clientsToBeRemoved.end(); it++)
	{
		std::map<int, Client>::iterator clientFound = clients.find(*it);

		if (clientFound == clients.end())
			continue ;

		std::cout << "\033[0;31mClient : " << clientFound->first << " disconnected...\033[0m" << std::endl;

		removeClientFromChannels(clientFound->second);

		close(clientFound->first);

		clients.erase(clientFound);

		FD_CLR(*it, &totalfds);
	}
}
