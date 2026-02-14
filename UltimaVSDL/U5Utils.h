#pragma once

#include <random>
#include <string>
#include <vector>

class U5Utils
{
public:
	explicit U5Utils();

	int GetRandom(int min, int max);
	std::vector<std::string> splitString(const std::string& str, char delimiter, bool keepDelim);
	void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);
private:
	std::random_device m_rd;
	std::mt19937 m_rGen;
};

