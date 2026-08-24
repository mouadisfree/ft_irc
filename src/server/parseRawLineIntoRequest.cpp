#include "../../include/server.hpp"
#include <sstream>

void	parseRawLineIntoRequest(const std::string &line, request &req)
{
	std::string	work = line;

	if (!work.empty() && work[work.size() - 1] == '\r')
		work.erase(work.size() - 1);

	size_t	start = 0;

	if (!work.empty() && work[0] == ':')
	{
		size_t	space = work.find(' ');

		if (space == std::string::npos)
			return;
		start = space + 1;
	}

	std::string	trailing;
	bool		hasTrailing = false;
	size_t		sep = work.find(" :", start);

	if (sep != std::string::npos)
	{
		trailing = work.substr(sep + 2);
		hasTrailing = true;
		work = work.substr(start, sep - start);
	}
	else
		work = work.substr(start);

	std::stringstream	iss(work);

	iss >> req.cmd;

	std::string	token;
	while (iss >> token)
		req.arg.push_back(token);

	if (hasTrailing)
		req.arg.push_back(trailing);
}
