#include "../../include/server.hpp"
#include <algorithm>

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
