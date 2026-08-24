#include "../../include/server.hpp"

Channel	*Server::findChannelByName(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);

    if (it == channels.end())
        return NULL;
    return it->second;
}
