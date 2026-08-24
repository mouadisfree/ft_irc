#include "../../include/server.hpp"

void Server::flushClient(Client &client)
{
    if (client.outBuffer.empty())
        return;

    ssize_t sent = send(client.socket_fd, client.outBuffer.c_str(), client.outBuffer.size(), 0);

    if (sent > 0)
    {
        client.outBuffer.erase(0, static_cast<size_t>(sent));
        return;
    }

    if (sent < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;

    client.step = C_CLOSE_CONNECTION;
}
