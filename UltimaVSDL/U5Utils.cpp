#include "U5Utils.h"
#include <random>

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