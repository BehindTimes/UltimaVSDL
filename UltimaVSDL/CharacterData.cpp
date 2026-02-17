#include "CharacterData.h"
#include <algorithm>
#include <cctype>

CharacterData::CharacterData() :
	m_name("Avatar")
{
	m_name_upper_case = m_name;
	std::transform(m_name_upper_case.begin(), m_name_upper_case.end(), m_name_upper_case.begin(),
		[](unsigned char c) { return static_cast<char>(std::toupper(c)); });
}