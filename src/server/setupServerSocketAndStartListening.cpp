#include "../../include/server.hpp"

void	Server::setupServerSocketAndStartListening()
{
	sockaddr_in address;

	address.sin_family = AF_INET;

	address.sin_port = htons(this->port);

	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Cannot create a socket.");

	int	reuse = 1;

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("Cannot set SO_REUSEADDR on the socket.");

	if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
		throw std::runtime_error(std::string("Cannot bind the socket: "));

	if (listen(server_fd, SOMAXCONN) < 0)
		throw std::runtime_error("Cannot listen to socket.");

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Cannot set the socket to non-blocking.");

	std::cout << "irc is listening at 0.0.0.0:" << port << std::endl;
}
