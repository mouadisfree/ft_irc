#include "../../headers/server.hpp"

static int	parsePort(std::string importedPort) {

	for (size_t i = 0; i < importedPort.size(); i++)
		if (!isdigit(importedPort[i])) return -1;

	int parsedPort = std::atoi(importedPort.c_str());

	if (parsedPort > USHRT_MAX) return -1;

	return (parsedPort);
}

void	Server::parseArgs(std::string importedPort, std::string importedPassword)
{
	port = parsePort(importedPort);

	if (port < 0) throw std::invalid_argument(importedPort + " is invalid PORT.");

	if (!importedPassword.size()) throw std::invalid_argument("No PASSWORD was given.");

	password = importedPassword;
}
