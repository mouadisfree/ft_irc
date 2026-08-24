#include "../../include/server.hpp"

void Server::handleQuitCommand(Client &client, request &p)
{
    if (!p.arg.empty() && !p.arg[0].empty())
        client.quitReason = p.arg[0];

    client.step = C_CLOSE_CONNECTION;
}
