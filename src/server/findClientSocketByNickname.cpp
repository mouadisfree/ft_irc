#include "../../include/server.hpp"

int Server::findClientSocketByNickname(request& req) {
    if (req.arg.empty())
        return -1;

    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.nickName == req.arg[0]) {
            return it->second.socket_fd;
        }
    }
    return -1;
}
