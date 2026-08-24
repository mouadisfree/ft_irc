#include "../../include/server.hpp"

void	acceptNewClientConnection(std::map<int, Client> &clients, fd_set &totalfds, int server_fd)
{
	sockaddr client_s;

	socklen_t client_length = sizeof(client_s);

	int newfd = accept(server_fd, &client_s, &client_length);

	if (newfd < 0) return;

	if (fcntl(newfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(newfd);
		return;
	}

	std::cout << "\033[1;32mClient : " << newfd << " connected...\033[0m" << std::endl;

	FD_SET(newfd, &totalfds);

	Client client;

	client.socket_fd = newfd;

	clients.insert(std::make_pair(newfd, client));
}
