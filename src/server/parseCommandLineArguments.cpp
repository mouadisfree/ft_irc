#include "../../include/server.hpp"

int	convertPortArgumentToPortNumber(const std::string &portArgument)
{
	for (size_t i = 0; i < portArgument.size(); i++)
		if (!isdigit(portArgument[i])) return -1;

	int parsedPort = std::atoi(portArgument.c_str());

	if (parsedPort > USHRT_MAX) return -1;

	return (parsedPort);
}

void	Server::parseCommandLineArguments(std::string importedPort, std::string importedPassword)
{
	port = convertPortArgumentToPortNumber(importedPort);

	if (port < 0) throw std::invalid_argument(importedPort + " is invalid PORT.");

	if (!importedPassword.size()) throw std::invalid_argument("No PASSWORD was given.");

	password = importedPassword;
}
