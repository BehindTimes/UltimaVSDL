#include "U5Utils.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>

U5Utils::U5Utils()
{
	m_rGen.seed(m_rd());
}

int U5Utils::GetRandom(int min, int max)
{
	int ret = 0;
	std::uniform_int_distribution<> distrib(min, max);
	ret = distrib(m_rGen);
	return ret;
}

std::vector<std::string> U5Utils::splitString(const std::string& str, char delimiter, bool keepDelim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	std::string strDelim("");
	strDelim += delimiter;
	bool init = true;

	if (str.size() > 0)
	{

		while (std::getline(ss, token, delimiter))
		{
			if (init)
			{
				init = false;
			}
			else
			{
				if (keepDelim)
				{
					tokens.push_back(strDelim);
				}
			}
			if (token.size() > 0)
			{
				tokens.push_back(token);
			}
		}
		if (keepDelim)
		{
			if (str.ends_with(delimiter))
			{
				tokens.push_back(strDelim);
			}
		}
	}
	return tokens;
}