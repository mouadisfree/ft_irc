#include "../../include/server.hpp"

bool Server::isClientAdminOfChannelInSecondArgument(request& req, Client& cli)
{
    std::map<std::string, Channel*>::iterator chanIt = channels.find(req.arg[1]);

    if (chanIt == channels.end())
        return false;

    std::vector<Client*>::iterator it;
    for (it = chanIt->second->admins.begin(); it != chanIt->second->admins.end(); ++it)
    {
        if ((*it)->nickName == cli.nickName)
            return true;
    }
    return false;
}
