#include "../../include/server.hpp"

bool Server::isClientOperatorOfChannelInFirstArgument(request& req, Client& cli)
{
    if (req.arg.empty())
        return false;
    return isClientChannelOperator(findChannelByName(req.arg[0]), cli);
}
