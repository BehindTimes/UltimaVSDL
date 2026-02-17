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

void U5Utils::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string U5Utils::trim(const std::string& str)
{
	const std::string WHITESPACE = " \n\r\t\f\v"; // Common whitespace characters
	size_t first = str.find_first_not_of(WHITESPACE);
	if (std::string::npos == first)
	{
		return "";
	}
	size_t last = str.find_last_not_of(WHITESPACE);
	return str.substr(first, (last - first + 1));
}

void U5Utils::SwapCharset(std::string& curString)
{
	for (size_t index = 0; index < curString.size(); index++)
	{
		unsigned char c = static_cast<unsigned char>(curString[index]);
		if (c >= 128)
		{
			c -= 128;
		}
		else
		{
			if (c != 0xa)
			{
				c += 128;
			}
		}

		curString[index] = static_cast<char>(c);

		if (index > 0)
		{
			if (c == 0xa7)
			{
				unsigned char d = static_cast<unsigned char>(curString[index - 1]);
				if (d == 0xa6)
				{
					char cline = static_cast<char>(0xec);
					curString[index - 1] = cline;
					curString[index] = cline;
				}
			}
		}
	}
}
