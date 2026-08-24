#include "../../include/server.hpp"

bool	Server::isClientChannelOperator(Channel *chan, Client &cli)
{
    if (chan == NULL)
        return false;

    for (std::vector<Client*>::iterator it = chan->admins.begin(); it != chan->admins.end(); ++it)
    {
        if (*it == &cli)
            return true;
    }
    return false;
}
