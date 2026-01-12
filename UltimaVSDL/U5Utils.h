#pragma once

#include <random>

class U5Utils
{
public:
	explicit U5Utils();

	int GetRandom(int min, int max);
private:
	std::random_device m_rd;
	std::mt19937 m_rGen;
};

