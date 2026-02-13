#include "U5World.h"
#include "GameObject.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include "GameBase.h"
#include "U5Enums.h"
#include <memory>
#include "U5Utils.h"
#include "U5Input.h"
#include <SDL3/SDL_keycode.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include "U5Game.h"
#include "ColorData.h"
#include <cstring>
#include <functional>
#include <string>
#include "GameOptions.h"
#include <SDL3/SDL_keyboard.h>

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;
extern std::unique_ptr<GameOptions> m_options;

U5World::U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameBase(sdl_helper, u5_resources),
	m_location_type(GameLocation::World),
	m_parent(nullptr),
	m_allowMove(true)
{
	//m_xpos = 50;
	//m_ypos = 50;
	//m_xpos = 86;
	//m_ypos = 110;
	//m_xpos = 176;
	//m_ypos = 210;
	
	m_xpos = 91;
	m_ypos = 68;

	//m_xpos = 148;
	//m_ypos = 83;

	// Shame
	//m_xpos = 58;
	//m_ypos = 103;

	m_DisplayOffset.first = 0;
	m_DisplayOffset.second = 0;

	m_smoothscroll = m_options->m_smooth_scroll;

	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
}

U5World::~U5World()
{
}

void U5World::SetParent(U5Game* parent)
{
	m_parent = parent;
}

void U5World::DrawNPCs()
{
	if (m_parent->m_curNPCs == nullptr)
	{
		return;
	}
	// Index 0 is the Avatar
	for (int index = 1; index < 32; index++)
	{
		if (m_parent->m_curNPCs->data[index].current_z != m_parent->m_cur_level)
		{
			continue;
		}
		int curpos = m_parent->m_curNPCs->data[index].type + 0x100;

		if (curpos >= m_sdl_helper->m_TileTextures.size() || curpos == 0x100)
		{
			continue; // Should never happen
		}

		int mapX = m_parent->m_curNPCs->data[index].current_x - (m_xpos - 6);
		int mapY = m_parent->m_curNPCs->data[index].current_y - (m_ypos - 6);

		unsigned char map_tile = m_parent->m_currentMap[m_parent->m_curNPCs->data[index].current_x][m_parent->m_curNPCs->data[index].current_y];

		SDL_Texture* curTexture;
		if (map_tile == BED_HEAD)
		{
			curTexture = m_sdl_helper->m_TileTextures[BED_SLEEPING].GetTexture();
		}
		else
		{
			curTexture = m_sdl_helper->m_TileTextures[curpos].GetTexture();
		}
		
		m_sdl_helper->DrawTileTexture(curTexture, mapX, mapY);
	}
}

void U5World::Render()
{
	DrawBorder();
	if (!m_vec_pos.empty())
	{
		ProcessScroll();
	}
	if (m_parent == nullptr)
	{
		return;
	}
	// We're going to render to the buffer, which gives us a 1 tile buffer on each side
	// This way, we can enable smooth scrolling in the future if we want
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();

	//unsigned char curpos = 0;

	if (nullptr == m_parent || m_parent->m_currentMap.empty() || m_parent->m_currentMap[0].empty())
	{
		return;
	}

	int mapX = m_xpos - 6;
	int mapY = m_ypos - 6;
	for (int ypos = 0; ypos < 13; ypos++)
	{
		for (int xpos = 0; xpos < 13; xpos++)
		{
			int curX;
			int curY;
			if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
			{
				curX = static_cast<int>((static_cast<unsigned long long>(mapX) + xpos + m_parent->m_currentMap.size()) % m_parent->m_currentMap.size());
				curY = static_cast<int>((static_cast<unsigned long long>(mapY) + ypos + m_parent->m_currentMap[0].size()) % m_parent->m_currentMap[0].size());
			}
			else
			{
				curX = mapX + xpos;
				curY = mapY + ypos;
				if (curX < 0 || curY < 0 || curX >= m_parent->m_currentMap.size() || curY >= m_parent->m_currentMap[0].size())
				{
					curX = 31;
					curY = 31;
				}
			}
			unsigned char curpos = m_parent->m_currentMap[curX][curY];
			SDL_Texture* curTexture = m_sdl_helper->m_TileTextures[curpos].GetTexture();
			m_sdl_helper->DrawTileTexture(curTexture, xpos, ypos);
		}
	}
	if (m_parent->m_location == GameLocation::Town)
	{
		DrawNPCs();
	}
}

int U5World::checkValidLocation(const PositionData& pos_info)
{
	if (m_parent->m_location == GameLocation::Town)
	{
		if (pos_info.new_position.first < 0 ||
			pos_info.new_position.second < 0 ||
			pos_info.new_position.first >= m_parent->m_currentMap.size() ||
			pos_info.new_position.second >= m_parent->m_currentMap[0].size())
		{
			m_process_key = std::bind(&U5World::ProcessLeaveTown, this);
			m_parent->m_console->BlockPrompt(true);
			return -1;
		}
	}
	return 0;
}

void U5World::HandleNorth()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(-1);
		}
	}
}

void U5World::HandleSouth()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(1);
		}
	}
}

void U5World::HandleEast()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(-1);
		}
	}
}

void U5World::HandleWest()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(1);
		}
	}
}

void U5World::HandleNortheast()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(-1);
		}
	}
}

void U5World::HandleNorthwest()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(1);
		}
	}
}

void U5World::HandleSoutheast()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(-1);
		}
	}
}

void U5World::HandleSouthwest()
{
	if (!m_allowMove)
	{
		return;
	}
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);

	if (m_parent->m_location == GameLocation::Town)
	{
		unsigned char curtile = m_parent->m_currentMap[m_xpos][m_ypos];
		if (curtile == STAIRS_UP)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_DOWN)
		{
			m_parent->ChangeLevel(1);
		}
		else if (curtile == STAIRS_LEFT)
		{
			m_parent->ChangeLevel(-1);
		}
		else if (curtile == STAIRS_RIGHT)
		{
			m_parent->ChangeLevel(1);
		}
	}
}

void U5World::ProcessNorth()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	m_vec_pos.back().new_position.first = m_xpos;
	tempval = m_ypos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap[0].size());
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	
	m_vec_pos.back().new_position.second = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NORTH_STRING]);
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleNorth, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessSouth()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	m_vec_pos.back().new_position.first = m_xpos;
	tempval = m_ypos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[SOUTH_STRING]);
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleSouth, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessEast()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	m_vec_pos.back().new_position.second = m_ypos;
	tempval = m_xpos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= m_parent->m_currentMap.size();
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[EAST_STRING]);
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleEast, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessWest()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	m_vec_pos.back().new_position.second = m_ypos;
	tempval = m_xpos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap.size());
		tempval %= static_cast<int>(m_parent->m_currentMap.size());
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WEST_STRING]);
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleWest, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessNorthEast()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap[0].size());
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= static_cast<int>(m_parent->m_currentMap.size());
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText("Northeast\n");
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleNortheast, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessNorthWest()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap[0].size());
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap.size());
		tempval %= static_cast<int>(m_parent->m_currentMap.size());
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText("Northwest\n");
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleNorthwest, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessSouthEast()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= m_parent->m_currentMap[0].size();
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= m_parent->m_currentMap.size();
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText("Southeast\n");
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleSoutheast, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessSouthWest()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval++;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempval += static_cast<int>(m_parent->m_currentMap.size());
		tempval %= static_cast<int>(m_parent->m_currentMap.size());
	}
	m_vec_pos.back().new_position.first = tempval;
	int check_location = checkValidLocation(m_vec_pos.back());
	if (0 < check_location)
	{
		m_vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
		m_parent->m_console->PrintText("Southwest\n");
		if (check_location == 0)
		{
			m_parent->m_console->NewPrompt();
			m_process_key = std::bind(&U5World::HandleSouthwest, this);
		}
		if (0 > check_location)
		{
			m_vec_pos.pop_back();
		}
	}
}

void U5World::ProcessLeaveTown()
{
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[LEAVE_TOWN_STRING]);
	m_process_key = std::bind(&U5World::HandleLeaveTown, this);
}

void U5World::PrintSign(int x, int y, int z)
{
	bool found = false;
	int tempindex = 0;
	for (int index = 0; index < m_parent->m_sign_data.size(); index++)
	{
		if (m_parent->m_sign_data[index].x == x && m_parent->m_sign_data[index].y == y && m_parent->m_sign_data[index].z == z)
		{
			tempindex = index;
			found = true;
			break;
		}
	}
	if (!found)
	{
		return;
	}
	if (m_parent->m_sign_data[tempindex].text != std::string("\n"))
	{
		m_parent->m_console->PrintText(m_parent->m_sign_data[tempindex].text, false, false, false);
	}
	else
	{
		if (m_parent->m_sign_data.size() > static_cast<size_t>(tempindex + 1))
		{
			m_parent->m_console->PrintText(m_parent->m_sign_data[tempindex + 1].text, false, false, false);
		}
	}
}

void U5World::HandleLook()
{
	int ret = ProcessDirection();
	int tempx = m_xpos;
	int tempy = m_ypos;

	m_input->m_isValid = false;

	switch (ret)
	{
	case 'U':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NORTH_STRING]);
		tempy--;
		break;
	case 'D':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[SOUTH_STRING]);
		tempy++;
		break;
	case 'L':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WEST_STRING]);
		tempx--;
		break;
	case 'R':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[EAST_STRING]);
		tempx++;
		break;
	default:
		return;
	}

	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
	m_input->SetRequireAllKeysUp();
	// Check for NPCs and items, then check the map tile

	// Finally, check the map tile
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		tempx = static_cast<int>((tempx + m_parent->m_currentMap.size()) % m_parent->m_currentMap.size());
		tempy = static_cast<int>((tempy + m_parent->m_currentMap[0].size()) % m_parent->m_currentMap[0].size());
	}
	else
	{
		if (tempx < 0 || tempy < 0 || tempx > 31 || tempy > 31)
		{
			tempx = 31;
			tempy = 31;
		}
	}
	int curpos = 0;

	// Check if NPC
	if (m_parent->m_curNPCs != nullptr)
	{
		for (int index = 1; index < 32; index++)
		{
			if (m_parent->m_curNPCs->data[index].current_x == tempx &&
				m_parent->m_curNPCs->data[index].current_y == tempy &&
				m_parent->m_curNPCs->data[index].current_z == m_parent->m_cur_level)
			{
				curpos = m_parent->m_curNPCs->data[index].type + 0x100;
				break;
			}
		}
	}

	if (curpos == 0)
	{
		curpos = m_parent->m_currentMap[tempx][tempy];
	}

	m_parent->m_console->PrintText(m_resources->m_data.game_strings[THOU_DOST_SEE_STRING]);

	std::string curItem;

	switch (curpos)
	{
	case 164:
	case 248:
		m_parent->m_console->PrintText("\n");
		PrintSign(tempx, tempy, m_parent->m_cur_level);
		break;
	default:
		curItem = m_resources->m_LookData[curpos];
		m_parent->m_console->PrintText(curItem);
		break;
	}
	
	/*if (curpos == 164 || curpos == 248) // sign
	{
		PrintSign(tempx, tempy, m_parent->m_cur_level);
	}*/
	m_parent->m_console->PrintText("\n");
	m_parent->m_console->NewPrompt();
}

void U5World::HandleKlimb()
{
	int ret = ProcessDirection();

	m_input->m_isValid = false;
	
	switch (ret)
	{
	case 'U':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NORTH_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		break;
	case 'D':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[SOUTH_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		break;
	case 'L':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WEST_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		break;
	case 'R':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[EAST_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		break;
	default:
		break;
	}
}

void U5World::HandleLeaveTown()
{
	int ret = ProcessYesNo();
	m_input->SetRequireAllKeysUp();
	if (ret == 'Y')
	{
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[YES_EXIT_STRING]);
		if (m_parent->m_old_location == GameLocation::World)
		{
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[BRITANNIA_STRING]);
			m_parent->m_console->NewPrompt();
			m_parent->LoadMap(-1);
		}
		else if (m_parent->m_old_location == GameLocation::Underworld)
		{
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[UNDERWORLD_STRING]);
			m_parent->m_console->NewPrompt();
			m_parent->LoadMap(-2);
		}
	}
	else if (ret == 'N')
	{
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NO_STRING]);
		m_parent->m_console->NewPrompt();
	}
}

int U5World::ProcessYesNo()
{
	m_input->m_isValid = true;
	SDL_Keycode curKey = m_input->GetCurrentKeyCode();
	switch (curKey)
	{
	case SDLK_Y:
		return 'Y';
	case SDLK_N:
		return 'N';
	default:
		m_input->m_isValid = false;
		break;
	}
	return -1;
}

int U5World::ProcessDirection()
{
	m_input->m_isValid = true;
	SDL_Keycode curKey = m_input->GetCurrentKeyCode();
	switch (curKey)
	{
	case SDLK_UP:
		return 'U';
		break;
	case SDLK_DOWN:
		return 'D';
		break;
	case SDLK_LEFT:
		return 'L';
		break;
	case SDLK_RIGHT:
		return 'R';
		break;
	default:
		m_input->m_isValid = false;
		break;
	}
	return -1;
}

void U5World::ProcessAnyKeyHit()
{
	if (m_input->isAnyKeyHit())
	{
		m_input->m_isValid = true;
		if (m_vec_pos.size() > 0)
		{
			return;
		}
		SDL_Keycode curKey = m_input->GetKeyCodePressed();
		switch (curKey)
		{
		case SDLK_KP_1:
			ProcessSouthWest();
			break;
		case SDLK_KP_3:
			ProcessSouthEast();
			break;
		case SDLK_KP_7:
			ProcessNorthWest();
			break;
		case SDLK_KP_9:
			ProcessNorthEast();
			break;
		case SDLK_UP:
			ProcessNorth();
			break;
		case SDLK_DOWN:
			ProcessSouth();
			break;
		case SDLK_LEFT:
			ProcessWest();
			break;
		case SDLK_RIGHT:
			ProcessEast();
			break;
		case SDLK_E:
			ProcessEnter();
			break;
		case SDLK_K:
			ProcessKlimb();
			break;
		case SDLK_L:
			ProcessLook();
			break;
		case SDLK_Y:
			ProcessYell();
			break;
		default:
			m_input->m_isValid = false;
			break;
		}
	}
}

void U5World::ProcessEvents()
{
	if (m_process_key)
	{
		m_process_key();
	}
}

void U5World::DrawBorder()
{
	SDL_FRect line;
	unsigned char color_array[3];

	if (m_resources->m_render_mode == RenderMode::CGA)
	{ 
		std::memcpy(color_array, cga_table[1], sizeof(color_array));
	}
	else
	{
		std::memcpy(color_array, ega_table[1], sizeof(color_array));
	}

	m_sdl_helper->DrawTileRect(1, 0, 6, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(1, 23, 6, 1, color_array[0], color_array[1], color_array[2]);

	m_sdl_helper->DrawTileRect(17, 0, 22, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(17, 23, 6, 1, color_array[0], color_array[1], color_array[2]);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 17, HALF_TILE_HEIGHT - LINE_THICKNESS, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH, HALF_TILE_HEIGHT * 23, (6 * HALF_TILE_WIDTH), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 17, HALF_TILE_HEIGHT * 23, (6 * HALF_TILE_WIDTH), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 16, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 16, 23);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 7, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 7, 23);

	GameBase::DrawBorder();

	if (m_xpos >= 100)
	{
		int tempval = (m_xpos / 100) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 8, 0);
	}
	if (m_xpos >= 10)
	{
		int tempval = (m_xpos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 9, 0);
	}
	int tempval1 = (m_xpos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval1)], 10, 0);

	if (m_ypos >= 100)
	{
		int tempval = (m_ypos / 100) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 12, 0);
	}
	if (m_ypos >= 10)
	{
		int tempval = (m_ypos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 13, 0);
	}
	int tempval2 = (m_ypos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval2)], 14, 0);
}

void U5World::ProcessScroll()
{
	// Should never happen
	if (m_vec_pos.empty())
	{
		return;
	}
	if (m_parent->m_currentMap.empty() || m_parent->m_currentMap[0].empty())
	{
		return;
	}

	if (m_smoothscroll)
	{
		m_vec_pos.back().elapsed_time += m_tickElapse;
		m_allowMove = m_vec_pos.back().elapsed_time >= m_vec_pos.back().TURN_TIME;
	}
	else
	{
		m_allowMove = true;
	}
	
	if (m_allowMove)
	{
		m_xpos = m_vec_pos.back().new_position.first;
		m_ypos = m_vec_pos.back().new_position.second;
		m_vec_pos.clear();
		m_DisplayOffset.first = 0;
		m_DisplayOffset.second = 0;
		m_input->EnableInput(true);
	}
	else
	{
		float ratio = static_cast<float>(m_vec_pos.back().elapsed_time) / m_vec_pos.back().TURN_TIME;
		float temppos1 = static_cast<float>(m_vec_pos.back().old_position.first);
		float temppos2 = static_cast<float>(m_vec_pos.back().new_position.first);

		int maxExtentX = static_cast<int>(m_parent->m_currentMap.size() - 1);
		int maxExtentY = static_cast<int>(m_parent->m_currentMap[0].size() - 1);

		if (m_vec_pos.back().old_position.first == 0 && m_vec_pos.back().new_position.first == maxExtentX)
		{
			temppos2 = -1;
		}
		else if (m_vec_pos.back().old_position.first == maxExtentX && m_vec_pos.back().new_position.first == 0)
		{
			temppos2 = maxExtentX + 1.0f;
		}
		m_DisplayOffset.first = (temppos1 - temppos2) * ratio;

		temppos1 = static_cast<float>(m_vec_pos.back().old_position.second);
		temppos2 = static_cast<float>(m_vec_pos.back().new_position.second);
		if (m_vec_pos.back().old_position.second == 0 && m_vec_pos.back().new_position.second == maxExtentY)
		{
			temppos2 = -1;
		}
		else if (m_vec_pos.back().old_position.second == maxExtentY && m_vec_pos.back().new_position.second == 0)
		{
			temppos2 = maxExtentY + 1.0f;
		}

		m_DisplayOffset.second = (temppos1 - temppos2) * ratio;
	}
}

void U5World::ProcessKlimb()
{
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[KLIMB_STRING]);
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[WITH_WHAT_STRING]);
		m_parent->m_console->NewPrompt();
		return;
	}
	if (m_xpos >= 0 && m_ypos >= 0 && m_xpos < m_parent->m_currentMap.size() && m_ypos < m_parent->m_currentMap[0].size())
	{
		unsigned char curpos = m_parent->m_currentMap[m_xpos][m_ypos];

		switch (curpos)
		{
		case LADDER_UP_TILE:
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[UP_STRING]);
			m_parent->m_console->NewPrompt();
			m_parent->ChangeLevel(1);
			return;
		case LADDER_DOWN_TILE:
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[DOWN_STRING]);
			m_parent->m_console->NewPrompt();
			m_parent->ChangeLevel(-1);
			return;
		default:
			break;
		}
	}
	m_process_key = std::bind(&U5World::HandleKlimb, this);
}

void U5World::ProcessLook()
{
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[LOOK_STRING]);
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[HYPHEN_STRING]);
	m_process_key = std::bind(&U5World::HandleLook, this);
}

void U5World::ProcessEnter()
{
	m_input->SetRequireAllKeysUp();

	if (m_location_type != GameLocation::World)
	{
		return;
	}
	std::pair<int, int> cur_location = {m_xpos, m_ypos};
	std::string dwelling_name;
	auto it = std::find(m_resources->m_data.location_info.begin(), m_resources->m_data.location_info.end(), cur_location);
	if (m_resources->m_data.location_info.end() != it)
	{
		size_t curMap = std::distance(m_resources->m_data.location_info.begin(), it);
		if (m_parent != nullptr)
		{
			m_parent->LoadMap(static_cast<int>(curMap));

			m_parent->m_console->PrintText(m_resources->m_data.game_strings[ENTER_STRING], false, true);
			if (m_resources->m_data.location_names[curMap].empty())
			{
				// Specially handle them
				switch (curMap)
				{
				case 0x0D: // Sutek's Hut
					dwelling_name = m_resources->m_data.game_strings[HUT_STRING];
					m_parent->m_console->PrintText(dwelling_name + std::string("\n"), false);
					break;
				case 0x0E: // Sin'Varaal's Hut
					dwelling_name = m_resources->m_data.game_strings[HUT_STRING];
					m_parent->m_console->PrintText(dwelling_name + std::string("\n"), false);
					break;
				case 0x0F: // Hut
					dwelling_name = m_resources->m_data.game_strings[HUT_STRING];
					m_parent->m_console->PrintText(dwelling_name + std::string("\n"), false);
					break;
				case 0x10: // Lord British
					dwelling_name = m_resources->m_data.game_strings[BRITISH_CASTLE_STRING];
					m_parent->m_console->PrintText(dwelling_name + std::string("\n"), false);
					break;
				case 0x11: // Lord Blackthorn
					dwelling_name = m_resources->m_data.game_strings[BLACKTHORN_PALANCE_STRING];
					m_parent->m_console->PrintText(dwelling_name + std::string("\n"), false);
					break;
				default:
					break;
				}
			}
			else
			{
				int dwellingType = static_cast<int>(curMap / 8);
				int exType = static_cast<int>(curMap % 8);
				
				switch (dwellingType)
				{
				case 0: // town
					dwelling_name = m_resources->m_data.game_strings[TOWN_STRING];
					break;
				case 1: // dwelling
					dwelling_name = exType < 4 ? m_resources->m_data.game_strings[LIGHTHOUSE_STRING] : m_resources->m_data.game_strings[HUT_STRING];
					break;
				case 2: // village
					dwelling_name = m_resources->m_data.game_strings[VILLAGE_STRING];
					break;
				case 3: // keep
					dwelling_name = m_resources->m_data.game_strings[KEEP_STRING];
					break;
				case 4: // dungeon
					dwelling_name = m_resources->m_data.game_strings[DUNGEON_STRING];
					break;
				default:
					dwelling_name = "???";
					break;
				}
				m_parent->m_console->PrintText(dwelling_name + std::string("\n\n"), false);
				int num_spaces = static_cast<int>(NUM_CONSOLE_WIDTH - m_resources->m_data.location_names[curMap].size()) / 2;
				for (int index = 0; index < num_spaces; index++)
				{
					m_parent->m_console->PrintText(" ", false);
				}
				m_parent->m_console->PrintText(m_resources->m_data.location_names[curMap]);
				m_parent->m_console->PrintText("\n", false);
			}
			m_parent->m_console->NewPrompt();
		}
	}
	else
	{
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[ENTER_WHAT]);
		m_parent->m_console->NewPrompt();
	}
}

// Calculate NPC stuff here
void U5World::PreMove()
{
}

// This will lead to an end turn
void U5World::PostMove()
{
}

void U5World::ProcessYell()
{
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[YELL_STRING]);
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_COLON_STRING]);
	m_process_key = std::bind(&U5World::HandleYell, this);
}

int U5World::ProcessLetterImmediate()
{
	int ret = -1;
	m_input->m_isValid = true;
	SDL_Keycode curKey = m_input->GetCurrentKeyCode();
	if (curKey >= SDLK_0 && curKey <= SDLK_9)
	{
		ret = static_cast<int>(curKey);
	}
	else if (curKey >= SDLK_A && curKey <= SDLK_Z)
	{
		ret = static_cast<int>(curKey);
	}
	else if (curKey == SDLK_RETURN || curKey == SDLK_BACKSPACE)
	{
		ret = static_cast<int>(curKey);
	}
	else if (curKey > SDLK_SPACE && curKey <= SDLK_TILDE)
	{
		ret = static_cast<int>(curKey);
	}
	else
	{
		ret = -1;
	}
	return ret;
}

void U5World::HandleYell()
{
	int ret = ProcessLetterImmediate();
	if (ret < 0)
	{
		return;
	}
	if (ret == SDLK_RETURN)
	{
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->NewPrompt();
		return;
	}
	else
	{
		const char* value = SDL_GetKeyName(ret);
		m_parent->m_console->PrintText(value);
	}
}
