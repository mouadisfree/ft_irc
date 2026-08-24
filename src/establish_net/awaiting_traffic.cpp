#include "../../headers/server.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <string>

void	acceptIncomingConnection(std::map<int, Client> &clients, fd_set &totalfds, int server_fd)
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

void	Server::removeClientFromChannels(Client &client)
{
	std::map<std::string, Channel*>::iterator it = channels.begin();

	while (it != channels.end())
	{
		Channel	*chan = it->second;

		if (chan == NULL)
		{
			std::map<std::string, Channel*>::iterator dead = it++;
			channels.erase(dead);
			continue ;
		}

		bool	wasMember = false;

		for (std::vector<Client*>::iterator m = chan->_members.begin(); m != chan->_members.end(); )
		{
			if (*m == &client)
			{
				wasMember = true;
				m = chan->_members.erase(m);
			}
			else
				++m;
		}

		for (std::vector<Client*>::iterator a = chan->admins.begin(); a != chan->admins.end(); )
		{
			if (*a == &client)
				a = chan->admins.erase(a);
			else
				++a;
		}

		if (wasMember && !client.nickName.empty())
		{
			std::string	quit = ":" + client.nickName + "!" + client.userName
				+ "@localhost QUIT :" + client.quitReason + "\r\n";

			for (std::vector<Client*>::iterator m = chan->_members.begin(); m != chan->_members.end(); ++m)
				queueMessage((*m)->socket_fd, quit);
		}

		if (chan->_members.empty())
		{
			std::map<std::string, Channel*>::iterator dead = it++;
			delete chan;
			channels.erase(dead);
			continue ;
		}

		if (chan->admin == &client)
		{
			chan->admin = chan->_members[0];

			if (std::find(chan->admins.begin(), chan->admins.end(), chan->admin) == chan->admins.end())
				chan->admins.push_back(chan->admin);
		}

		++it;
	}
}

void	Server::clearClients(std::vector<int> BeRemoved, fd_set &totalfds)
{
	for (std::vector<int>::iterator it = BeRemoved.begin(); it != BeRemoved.end(); it++)
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

static void	parseLine(const std::string &line, request &req)
{
	std::string	work = line;

	if (!work.empty() && work[work.size() - 1] == '\r')
		work.erase(work.size() - 1);

	size_t	start = 0;

	if (!work.empty() && work[0] == ':')
	{
		size_t	space = work.find(' ');

		if (space == std::string::npos)
			return;
		start = space + 1;
	}

	std::string	trailing;
	bool		hasTrailing = false;
	size_t		sep = work.find(" :", start);

	if (sep != std::string::npos)
	{
		trailing = work.substr(sep + 2);
		hasTrailing = true;
		work = work.substr(start, sep - start);
	}
	else
		work = work.substr(start);

	std::stringstream	iss(work);

	iss >> req.cmd;

	std::string	token;
	while (iss >> token)
		req.arg.push_back(token);

	if (hasTrailing)
		req.arg.push_back(trailing);
}

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
		parseLine(line, req);

		if (req.cmd.empty())
			continue;

		if (getAuthentified(client, req) == 3)
		{
			if (client.authenticated == false)
			{
				send_message(client.socket_fd, RPL_WELCOME(client.nickName));
				send_message(client.socket_fd, RPL_YOURHOST(client.nickName, std::string("irc.server.com")));
				send_message(client.socket_fd, RPL_CREATED(client.nickName));
				send_message(client.socket_fd, RPL_MYINFO(client.nickName, std::string("irc.server.com")));
				std::cout << client.nickName << " Welcome to irc server!" << std::endl;
				client.authenticated = true;
			}
		}
	}
}

void	Server::awaitingTraffic()
{
	fd_set totalfds, readfds, writefds;

	FD_ZERO(&totalfds);

	FD_SET(server_fd, &totalfds);

	while(true) {

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

		if (FD_ISSET(server_fd, &readfds)) acceptIncomingConnection(this->clients, totalfds, server_fd);

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step != C_CLOSE_CONNECTION && FD_ISSET(it->first, &readfds))
				handleReadRequest(it->second);
		}

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step != C_CLOSE_CONNECTION && FD_ISSET(it->first, &writefds))
				flushClient(it->second);
		}

		std::vector<int> clientsReadyToBeRemoved;

		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.step == C_CLOSE_CONNECTION)
				clientsReadyToBeRemoved.push_back(it->first);
		}

		clearClients(clientsReadyToBeRemoved, totalfds);
	}
}
