#include "../../include/server.hpp"

bool Server::isChannelMemberLimitReached(request& req, int *user)
{
    (void)user;

    Channel *chan = findChannelByName(req.arg[0]);

    if (chan == NULL || !chan->isLimit)
        return false;

    return (static_cast<int>(chan->_members.size()) >= chan->maxsize);
}
