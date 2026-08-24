#include "../../include/server.hpp"

void	Server::parseCommandLineArguments(std::string importedPort, std::string importedPassword)
{
	port = convertPortArgumentToPortNumber(importedPort);

	if (port < 0) throw std::invalid_argument(importedPort + " is invalid PORT.");

	if (!importedPassword.size()) throw std::invalid_argument("No PASSWORD was given.");

	password = importedPassword;
}
