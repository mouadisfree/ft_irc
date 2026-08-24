#include "../../include/server.hpp"

int	convertPortArgumentToPortNumber(const std::string &portArgument)
{
	for (size_t i = 0; i < portArgument.size(); i++)
		if (!isdigit(portArgument[i])) return -1;

	int parsedPort = std::atoi(portArgument.c_str());

	if (parsedPort > USHRT_MAX) return -1;

	return (parsedPort);
}
