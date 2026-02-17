#pragma once

#include <string>

class CharacterData
{
public:
	explicit CharacterData();
	std::string m_name;
	std::string m_name_upper_case;
	
	bool m_knowAvatar = false; // TO DO: Create a character record for every NPC
private:
};

