#include "../../include/server.hpp"
#include <cstring>

void    Server::handleReadRequest(Client &client)
{
    char buf[1024];

	std::memset(buf, 0, sizeof(buf));
    int bytes_received = recv(client.socket_fd, buf, sizeof(buf) - 1, 0);

	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;

		client.step = C_CLOSE_CONNECTION;
		return;
	}

	if (bytes_received == 0)
	{
		client.step = C_CLOSE_CONNECTION;
		return;
	}

	buf[bytes_received] = '\0';
	client.buffer += buf;

	size_t pos;
	while ((pos = client.buffer.find_first_of("\r\n")) != std::string::npos)
	{
		std::string line = client.buffer.substr(0, pos);
		client.buffer.erase(0, pos + 1);

		if (line.empty())
			continue;

		request req;
		parseRawLineIntoRequest(line, req);

		if (req.cmd.empty())
			continue;

		if (dispatchClientCommand(client, req) == 3)
		{
			if (client.authenticated == false)
			{
				sendMessageToOneClient(client.socket_fd, RPL_WELCOME(client.nickName));
				sendMessageToOneClient(client.socket_fd, RPL_YOURHOST(client.nickName, std::string("irc.server.com")));
				sendMessageToOneClient(client.socket_fd, RPL_CREATED(client.nickName));
				sendMessageToOneClient(client.socket_fd, RPL_MYINFO(client.nickName, std::string("irc.server.com")));
				std::cout << client.nickName << " Welcome to irc server!" << std::endl;
				client.authenticated = true;
			}
		}
	}
}
