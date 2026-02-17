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
#include "CharacterData.h"

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;
extern std::unique_ptr<GameOptions> m_options;
extern std::unique_ptr<CharacterData> m_charData;

U5World::U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameBase(sdl_helper, u5_resources),
	m_location_type(GameLocation::World),
	m_parent(nullptr),
	m_allowMove(true),
	m_currentDialog(-1),
	m_current_talk_pause_delay(0)
{
	// blackthorn
	//m_xpos = 196;
	//m_ypos = 245;

	// serpent's hold
	//m_xpos = 146;	// 92
	//m_ypos = 242; // F2

	// 
	//m_xpos = 50;
	//m_ypos = 50;
	m_xpos = 83;  // 53
	m_ypos = 106; // 6A
	//m_xpos = 176;
	//m_ypos = 210;
	
	//m_xpos = 91;
	//m_ypos = 68;

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
		int mapXMin = m_xpos - 6;
		int mapYMin = m_ypos - 6;
		int mapXMax = m_xpos + 6;
		int mapYMax = m_ypos + 6;

		if (m_parent->m_curNPCs->data[index].current_x < mapXMin ||
			m_parent->m_curNPCs->data[index].current_x > mapXMax ||
			m_parent->m_curNPCs->data[index].current_y < mapYMin ||
			m_parent->m_curNPCs->data[index].current_y > mapYMax ||
			m_parent->m_curNPCs->data[index].current_z != m_parent->m_cur_level)
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

void U5World::DrawAvatar()
{
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER]);

	int xpos = 6;
	int ypos = 6;

	SDL_Texture* curTexture;

	curTexture = m_sdl_helper->m_TileTextures[0x11c].GetTexture();
	m_sdl_helper->DrawTileTexture(curTexture, xpos, ypos, m_DisplayOffset);

	m_sdl_helper->SetRenderTarget(nullptr);
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
	DrawAvatar();
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
		// TO DO: Should print Live by the eight laws.
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
	case ' ':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[PASS_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
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
	case 160:
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
	case ' ':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[PASS_STRING]);
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
	case SDLK_SPACE:
		return ' ';
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
		case SDLK_T:
			ProcessTalk();
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
	m_displayWord.clear();
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[YELL_STRING]);
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[WHAT_COLON_STRING]);
	m_process_key = std::bind(&U5World::StartYell, this);
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
	else if (curKey >= SDLK_SPACE && curKey <= SDLK_TILDE)
	{
		ret = static_cast<int>(curKey);
	}
	else
	{
		ret = -1;
	}
	return ret;
}

bool U5World::DoYell()
{
	return false;
}

void U5World::StartYell()
{
	if (!m_parent->m_console->IsReady())
	{
		return;
	}
	m_parent->m_console->StartLineEdit();
	m_process_key = std::bind(&U5World::HandleYell, this);
}

void U5World::StartTalk()
{
	if (!m_parent->m_console->IsReady())
	{
		return;
	}
	m_parent->m_console->StartLineEdit();
	m_process_key = std::bind(&U5World::HandleTalkInput, this);
}

void U5World::StartNameTalk()
{
	if (!m_parent->m_console->IsReady())
	{
		return;
	}
	m_parent->m_console->StartLineEdit();
	m_process_key = std::bind(&U5World::HandleNameTalkInput, this);
}


void U5World::HandleYell()
{
	int ret = ProcessLetterImmediate();
	m_input->m_isValid = false;
	if (ret < 0)
	{
		return;
	}
	if (ret == SDLK_RETURN)
	{
		bool allowNewLine = m_parent->m_console->LineWasIncremented();
		const int MAX_CONSOLE = 16;
		bool drawNewLine = true;
		if (!allowNewLine && m_displayWord.size() < MAX_CONSOLE)
		{
			drawNewLine = false;
		}
		if (m_displayWord.empty())
		{
			m_parent->m_console->PrintEditText(m_resources->m_data.game_strings[NOTHING_STRING]);
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			else
			{
				// Useless unless someone decides to edit the text without a newline
				// Just include for that case
				m_parent->m_console->SetCursorStartPosX(0);
			}
		}
		else
		{
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			else
			{
				// Useless unless someone decides to edit the text without a newline
				// Just include for that case
				m_parent->m_console->SetCursorStartPosX(0);
			}
			if (!DoYell())
			{
				m_parent->m_console->PrintText(m_resources->m_data.game_strings[NO_EFFECT_STRING]);
			}
		}
		
		m_parent->m_console->EndLineEdit();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->NewPrompt();
		return;
	}
	else if (ret == SDLK_BACKSPACE)
	{
		if (!m_displayWord.empty())
		{
			m_displayWord.pop_back();
			m_parent->m_console->PrintEditText(m_displayWord);
		}
	}
	else
	{
		const char* value = SDL_GetKeyName(ret);
		if (value != 0)
		{
			if (m_displayWord.size() < MAX_YELL)
			{
				std::string curLetter;

				if (ret == SDLK_SPACE)
				{
					m_displayWord += ' ';
					curLetter += ' ';
				}
				else
				{
					m_displayWord += value[0];
					curLetter += value[0];
				}

				m_parent->m_console->PrintEditText(m_displayWord);
			}
		}
	}
}

void U5World::ProcessTalk()
{
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[TALK_STRING]);
	m_displayWord.clear();
	m_input->SetRequireAllKeysUp();
	m_process_key = std::bind(&U5World::HandleTalk, this);
}

void U5World::HandleTalk()
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
	case ' ':
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[PASS_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		return;
	default:
		return;
	}
	if (m_parent->m_location == GameLocation::World || m_parent->m_location == GameLocation::Underworld)
	{
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[FUNNY_NO_RESPONSE_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		return;
	}

	if (tempx < 0 || tempy < 0 || tempx > 31 || tempy > 31)
	{
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NOBODY_HERE]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		return;
	}

	int curpos = m_parent->m_currentMap[tempx][tempy];
	if ((curpos >= 154 && curpos <= 156) ||
		(curpos >= 148 && curpos <= 150))
	{
		if (ret == 'U')
		{
			tempy--;
		}
		else if (ret == 'D')
		{
			tempy++;
		}
	}
	
	int dialog_pos = -1;
	int npc_type = -1;
	// Check if NPC
	if (m_parent->m_curNPCs != nullptr)
	{
		for (int index = 1; index < 32; index++)
		{
			if (m_parent->m_curNPCs->data[index].current_x == tempx &&
				m_parent->m_curNPCs->data[index].current_y == tempy &&
				m_parent->m_curNPCs->data[index].current_z == m_parent->m_cur_level)
			{
				dialog_pos = m_parent->m_curNPCs->data[index].dialog_number;
				npc_type = m_parent->m_curNPCs->data[index].type;
				break;
			}
		}
	}

	if (dialog_pos < 0)
	{
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NOBODY_HERE]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		return;
	}

	int currentDialog = dialog_pos;


	if (currentDialog == 0)
	{
		// Guards offer a special no response
		if (npc_type >= 0x70 && npc_type < 0x74)
		{
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[GUARD_NO_RESPONSE_STRING]);
		}
		else
		{
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->PrintText(m_resources->m_data.game_strings[NO_RESPONSE_STRING]);
		}
		
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
		return;
	}
	m_currentDialog = DialogInfo(currentDialog);

	if (!m_parent->m_talk_data.contains(m_currentDialog.dialog_num))
	{
		// TO DO: This is filler just to prevent locking up for the time being
		m_parent->m_console->PrintText("\n");
		m_parent->m_console->PrintText(m_resources->m_data.game_strings[NO_RESPONSE_STRING]);
		m_parent->m_console->NewPrompt();
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_input->SetRequireAllKeysUp();
	}
	else
	{
		m_currentDialog.dialog = m_parent->m_talk_data[m_currentDialog.dialog_num];
		m_currentDialog.current_instruction = m_currentDialog.dialog.description;
		m_process_key = std::bind(&U5World::DoTalk, this);
	}
}

void U5World::ProcessTalkPause()
{
	m_current_talk_pause_delay += m_tickElapse;
	if (m_current_talk_pause_delay > TALK_PAUSE_TIME)
	{
		m_parent->m_console->SetCursorVisible(true);
		m_process_key = std::bind(&U5World::DoTalk, this);
	}
}

void U5World::ProcessTalkKeyWait()
{
	int ret = ProcessLetterImmediate();
	if (ret > 0)
	{
		m_process_key = std::bind(&U5World::DoTalk, this);
	}
}

void U5World::DoTalk()
{
	bool knowAvatar = m_charData->m_knowAvatar;
	bool knowName = false;
	int karma = 76;
	bool firstinstruction = true;
	bool runemode = false;

	for (m_currentDialog.instruction_num; m_currentDialog.instruction_num < m_currentDialog.current_instruction.size(); m_currentDialog.instruction_num++)
	{
		if (m_currentDialog.instruction_num == 0)
		{
			switch (m_currentDialog.mode)
			{
			case TalkMode::Name:
				m_parent->m_console->PrintText(m_resources->m_data.game_strings[MY_NAME_IS_STING]);
				break;
			case TalkMode::Description:
				m_parent->m_console->PrintText("\n");
				m_parent->m_console->PrintText(m_resources->m_data.game_strings[YOU_SEE_STRING]);
				break;
			case TalkMode::Greeting:
				m_parent->m_console->PrintText("\n\n");
				if (!knowAvatar)
				{
					if (knowName)
					{
						m_parent->m_console->PrintText(m_resources->m_data.game_strings[I_AM_CALLED]);
						m_parent->m_console->PrintText(m_currentDialog.dialog.name[0].second);
						if(m_currentDialog.dialog.name[0].second.size() > 0 &&
							m_currentDialog.dialog.name[0].second.back() != '\"')
						{
							m_parent->m_console->PrintText("\"");
						}
						m_parent->m_console->PrintText("\n\n");
					}
					m_currentDialog.label_num = -1;
					ProcessTalkInput();
					return;
				}
				break;
			default:
				break;
			};
		}

		std::string printWord;
		int value;

		switch (m_currentDialog.current_instruction[m_currentDialog.instruction_num].first)
		{
		case -1: // Default prompt case
			break;
		case 0:
			printWord.clear();
			if (m_currentDialog.mode == TalkMode::Goodbye || m_currentDialog.mode == TalkMode::Label || m_currentDialog.mode == TalkMode::Greeting)
			{
				if (firstinstruction)
				{
					if (m_currentDialog.current_instruction[m_currentDialog.instruction_num].second.size() > 0 &&
						m_currentDialog.current_instruction[m_currentDialog.instruction_num].second[0] != '\"')
					{
						printWord += "\"";
					}
				}
			}

			// Technically, you can have the full thing stay in rune mode, but this never actually occurs in game.
			// Once again, this is different than how the actual game handles things, but works the same way,
			// provided the talk script stays in the proper form
			if (runemode)
			{
				std::string strOut = m_currentDialog.current_instruction[m_currentDialog.instruction_num].second;
				m_utilities->SwapCharset(strOut);
				printWord += strOut;
			}
			else
			{
				printWord += m_currentDialog.current_instruction[m_currentDialog.instruction_num].second;
			}
			
			if (m_currentDialog.mode == TalkMode::Goodbye || m_currentDialog.mode == TalkMode::Name)
			{
				if (m_currentDialog.instruction_num == m_currentDialog.current_instruction.size() - 1)
				{
					printWord += m_resources->m_data.game_strings[END_QUOTED_STRING];
				}
			}
			else if (m_currentDialog.mode == TalkMode::Label)
			{
				if (m_currentDialog.instruction_num == m_currentDialog.current_instruction.size() - 1)
				{
					printWord += m_resources->m_data.game_strings[END_QUOTED_STRING];
					printWord += '\n';
				}
			}
			else if (m_currentDialog.mode == TalkMode::Greeting)
			{
				if (m_currentDialog.instruction_num == m_currentDialog.current_instruction.size() - 1)
				{
					printWord += "\"";
				}
			}
			m_parent->m_console->PrintText(printWord);
			firstinstruction = false;
			break;
		case 0x81: // print Avatar name
			if (firstinstruction)
			{
				m_parent->m_console->PrintText("\"");
				firstinstruction = false;
			}
			m_parent->m_console->PrintText(m_charData->m_name);
			break;
		case 0x82: // end conversation
			m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
			m_parent->m_console->NewPrompt();
			return;
			break;
		case 0x83: // pause
			m_process_key = std::bind(&U5World::ProcessTalkPause, this);
			m_current_talk_pause_delay = 0;
			m_currentDialog.instruction_num++;
			m_parent->m_console->SetCursorVisible(false);
			return;
			break;
		case 0x84: // join party
			m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
			m_parent->m_console->NewPrompt();
			return;
			break;
		case 0x88: // ask name
			printWord.clear();
			if (firstinstruction)
			{
				printWord += "\"";
				firstinstruction = false;
			}
			printWord += m_resources->m_data.game_strings[PROMPT_NAME_STRING];
			printWord += "\n";
			m_parent->m_console->PrintText(printWord);
			ProcessNameInput();
			return;
			break;
		case 0x8c: // if/else
			if (!knowAvatar)
			{
				m_currentDialog.instruction_num++;
			}
			break;
		case 0x8d: // New line
			m_parent->m_console->PrintText("\n");
			firstinstruction = true;
			break;
		case 0x8e: // rune mode
			runemode = !runemode;
			break;
		case 0x8f: // key wait
			m_process_key = std::bind(&U5World::ProcessTalkKeyWait, this);
			m_currentDialog.instruction_num++;
			return;
			break;
		case 0x91: // goto label
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
		case 0x98:
		case 0x99:
		case 0x9a:
			m_currentDialog.mode = TalkMode::Label;
			m_currentDialog.label_num = m_currentDialog.current_instruction[m_currentDialog.instruction_num].first;
			if (m_currentDialog.dialog.labels.contains(m_currentDialog.label_num))
			{
				m_currentDialog.instruction_num = 0;
				m_currentDialog.current_instruction = m_currentDialog.dialog.labels[m_currentDialog.label_num].text;
				return;
			}
			break;
		case 0xfe:// karma check
			if (m_currentDialog.instruction_num + 3 < m_currentDialog.current_instruction.size())
			{
				m_currentDialog.instruction_num++;
				value = m_currentDialog.current_instruction[m_currentDialog.instruction_num].first;
				if (karma < value)
				{
					m_currentDialog.instruction_num++;
				}
			}
			break;
		case 0xff:
			m_currentDialog.instruction_num = m_currentDialog.current_instruction.size();
			switch (m_currentDialog.mode)
			{
			case TalkMode::Name:
				m_currentDialog.mode = TalkMode::Label;
				m_currentDialog.label_num = -1;
				break;
			case TalkMode::Description:
				m_currentDialog.mode = TalkMode::Label;
				m_currentDialog.label_num = -1;
				//ProcessTalkInput();
				return;
				break;
			case TalkMode::Label:
				m_currentDialog.label_num = -1;
				break;
			default:
				break;
			};
			break;
		default:
		{
			int j = 9;
		}
			break;
		};
	}

	switch (m_currentDialog.mode)
	{
	case TalkMode::Name:
		m_parent->m_console->PrintText("\n");
		m_currentDialog.mode = TalkMode::Label;
		m_currentDialog.label_num = -1;
		ProcessTalkInput();
		break;
	case TalkMode::Description:
		m_currentDialog.mode = TalkMode::Greeting;
		m_currentDialog.instruction_num = 0;
		m_currentDialog.current_instruction = m_currentDialog.dialog.greeting;
		m_currentDialog.label_num = -1;
		break;
	case TalkMode::Greeting:
		m_parent->m_console->PrintText("\n\n");
		m_currentDialog.label_num = -1;
		ProcessTalkInput();
		break;
	case TalkMode::Label:
		//m_parent->m_console->PrintText("\n");
		if (m_currentDialog.label_num >= 0x91 && m_currentDialog.label_num <= 0x9a)
		{
			ProcessLabelInput();
		}
		else
		{
			ProcessTalkInput();
		}
		break;
	case TalkMode::Goodbye:
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->NewPrompt();
		break;
	default:
		break;
	};
}

void U5World::ProcessTalkInput()
{
	m_displayWord.clear();
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[YOUR_INTEREST]);
	m_process_key = std::bind(&U5World::StartTalk, this);
}

void U5World::ProcessLabelInput()
{
	m_displayWord.clear();
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[YOU_RESPOND_STRING]);
	m_process_key = std::bind(&U5World::StartTalk, this);
}

void U5World::ProcessNameInput()
{
	m_displayWord.clear();
	m_input->SetRequireAllKeysUp();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings[YOU_RESPOND_STRING]);
	m_process_key = std::bind(&U5World::StartNameTalk, this);
}

void U5World::HandleNameTalkWord(std::string strReponse)
{
	strReponse = m_utilities->trim(strReponse);
	const int MAX_CONSOLE = 16;

	bool allowNewLine = m_parent->m_console->LineWasIncremented();
	bool drawNewLine = true;
	if (!allowNewLine && strReponse.size() < MAX_CONSOLE)
	{
		drawNewLine = false;
	}

	m_process_key = std::bind(&U5World::DoTalk, this);
	
	m_parent->m_console->EndLineEdit();

	m_currentDialog.instruction_num++;

	std::string responsestring;

	if (strReponse == m_charData->m_name_upper_case)
	{
		// TO DO: associate this with the character.  Temporarily, making this global
		m_charData->m_knowAvatar = true;
		responsestring = m_resources->m_data.game_strings[A_PLEASURE_STRING];
	}
	else
	{
		responsestring = m_resources->m_data.game_strings[IF_YOU_SAY_SO_STRING];
	}

	// Another hack to fix the console.  Technically, without this, it matches the original
	// game, but this will make it look cleaner.  Once again though, things could mess
	// up if someone edits the talk script without matching the original quirks
	if (responsestring.starts_with("\n"))
	{
		responsestring.erase(0, 1);
	}

	if (drawNewLine)
	{
		m_parent->m_console->PrintText("\n");
	}

	m_parent->m_console->PrintText(responsestring);
}

void U5World::HandleTalkWord(std::string strReponse)
{
	strReponse = m_utilities->trim(strReponse);
	const int MAX_CONSOLE = 16;

	bool allowNewLine = m_parent->m_console->LineWasIncremented();
	bool drawNewLine = true;
	if (!allowNewLine && strReponse.size() < MAX_CONSOLE)
	{
		drawNewLine = false;
	}

	// Check for naughty words
	for (size_t index = INPUT_NAUGHTY; index < m_resources->m_data.input_words.size(); index++)
	{
		if (strReponse.starts_with(m_resources->m_data.input_words[index]))
		{
			m_process_key = std::bind(&U5World::DoTalk, this);
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->EndLineEdit();
			m_currentDialog.mode = TalkMode::Label;
			std::vector<std::pair<int, std::string>> naughty_instructions;
			naughty_instructions.push_back(std::make_pair(0, m_resources->m_data.game_strings[WITH_LANGUAGE_LIKE_THAT_STRING] + "\"\n\n"));
			naughty_instructions.push_back(std::make_pair(0x83, "")); // Add pause
			naughty_instructions.push_back(std::make_pair(m_currentDialog.label_num, ""));
			m_currentDialog.current_instruction = naughty_instructions;
			m_currentDialog.instruction_num = 0;
			return;
		}
	}

	if (m_currentDialog.label_num == -1)
	{
		if (strReponse.empty() || strReponse.starts_with(m_resources->m_data.input_words[INPUT_BYE]) || strReponse.starts_with(m_resources->m_data.input_words[INPUT_THANK]))
		{
			m_process_key = std::bind(&U5World::DoTalk, this);
			m_currentDialog.mode = TalkMode::Goodbye;
			m_currentDialog.instruction_num = 0;
			m_currentDialog.current_instruction = m_currentDialog.dialog.bye;
			if (strReponse.empty())
			{
				m_parent->m_console->PrintEditText(m_resources->m_data.game_strings[BYE_STRING]);
			}
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->EndLineEdit();
			return;
		}
		else if (strReponse.starts_with(m_resources->m_data.input_words[INPUT_NAME]))
		{
			m_process_key = std::bind(&U5World::DoTalk, this);
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->EndLineEdit();
			m_currentDialog.mode = TalkMode::Name;
			m_currentDialog.instruction_num = 0;
			m_currentDialog.current_instruction = m_currentDialog.dialog.name;
			return;
		}
		else if (strReponse.starts_with(m_resources->m_data.input_words[INPUT_JOB]) || strReponse.starts_with(m_resources->m_data.input_words[INPUT_WORK]))
		{
			m_process_key = std::bind(&U5World::DoTalk, this);
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->EndLineEdit();
			m_currentDialog.instruction_num = 0;
			m_currentDialog.mode = TalkMode::Label;
			m_currentDialog.label_num = -1;
			m_currentDialog.current_instruction = m_currentDialog.dialog.job;
			return;
		}
		else
		{
			// Originally, I used a map, but the game searches in order.  This actually leads
			// to a bug where a keyword could prevent another keyword, but we're going to keep
			// that here to be consistent.
			for (size_t index = 0; index < m_currentDialog.dialog.keywords.size(); index++)
			{
				if (strReponse.starts_with(m_currentDialog.dialog.keywords[index].first))
				{
					m_process_key = std::bind(&U5World::DoTalk, this);
					if (drawNewLine)
					{
						m_parent->m_console->PrintText("\n");
					}
					m_parent->m_console->PrintText("\n");
					m_parent->m_console->EndLineEdit();
					m_currentDialog.instruction_num = 0;
					m_currentDialog.mode = TalkMode::Label;
					m_currentDialog.label_num = -1;
					m_currentDialog.current_instruction = m_currentDialog.dialog.keywords[index].second;
					return;
				}
			}
			m_process_key = std::bind(&U5World::DoTalk, this);
			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			m_parent->m_console->PrintText("\n");
			m_parent->m_console->EndLineEdit();
			m_currentDialog.instruction_num = 0;
			m_currentDialog.mode = TalkMode::Label;
			m_currentDialog.label_num = -1;
			std::vector<std::pair<int, std::string>> default_instructions;
			default_instructions.push_back(std::make_pair(0, m_resources->m_data.game_strings[CANNOT_HELP]));
			m_currentDialog.current_instruction = default_instructions;
			return;
		}
	}
	else
	{
		if (strReponse.empty())
		{
			m_process_key = std::bind(&U5World::DoTalk, this);
			
			// Correcting for a bug, which does not show a quotation when it should
			std::string what_did_you_say = m_resources->m_data.game_strings[WHAT_DID_YOU_SAY_STRING];
			
			if (what_did_you_say.ends_with('\"'))
			{
				what_did_you_say += '\"';
			}
			// I'm putting in this hack, or else I'd need to rewrite the console.
			// With empty phrases, the game expects no return value, and will
			// require it in the string.  I believe this is the only area this
			// can occur, as yelling and normal conversation all place in a fake word
			// Not that you'd want to have a response on the line, even with
			// translations, but in the original you could, and this won't perfectly
			// replicate it.
			if (what_did_you_say.starts_with("\n"))
			{
				what_did_you_say.erase(0, 1);
			}
			what_did_you_say += '\"';

			if (drawNewLine)
			{
				m_parent->m_console->PrintText("\n");
			}
			what_did_you_say += "\n\n";

			m_parent->m_console->PrintText(what_did_you_say);
			m_parent->m_console->EndLineEdit();
			return;
		}
		else
		{
			if (m_currentDialog.dialog.labels.contains(m_currentDialog.label_num))
			{
				// Originally, I used a map, but the game searches in order.  This actually leads
				// to a bug where a keyword could prevent another keyword, but we're going to keep
				// that here to be consistent.
				for (size_t index = 0; index < m_currentDialog.dialog.labels[m_currentDialog.label_num].keywords.size(); index++)
				{
					if (strReponse.starts_with(m_currentDialog.dialog.labels[m_currentDialog.label_num].keywords[index].first))
					{
						m_process_key = std::bind(&U5World::DoTalk, this);
						if (drawNewLine)
						{
							m_parent->m_console->PrintText("\n");
						}
						m_parent->m_console->PrintText("\n");
						m_parent->m_console->EndLineEdit();
						m_currentDialog.instruction_num = 0;
						m_currentDialog.mode = TalkMode::Label;
						m_currentDialog.current_instruction = m_currentDialog.dialog.labels[m_currentDialog.label_num].keywords[index].second;
						m_currentDialog.label_num = -1;
						return;
					}
				}
				m_process_key = std::bind(&U5World::DoTalk, this);
				if (drawNewLine)
				{
					m_parent->m_console->PrintText("\n");
				}
				m_parent->m_console->PrintText("\n");
				m_parent->m_console->EndLineEdit();
				m_currentDialog.instruction_num = 0;
				m_currentDialog.mode = TalkMode::Label;
				m_currentDialog.current_instruction = m_currentDialog.dialog.labels[m_currentDialog.label_num].default_answer;
				m_currentDialog.label_num = -1;
				return;
			}
		}
	}
	
	// A default case for corrupt scripts
	m_parent->m_console->EndLineEdit();
	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
	m_parent->m_console->NewPrompt();
}

void U5World::HandleTalkInput()
{
	int ret = ProcessLetterImmediate();
	m_input->m_isValid = false;
	if (ret < 0)
	{
		return;
	}
	if (ret == SDLK_RETURN)
	{
		HandleTalkWord(m_displayWord);
		return;
	}
	else if (ret == SDLK_BACKSPACE)
	{
		if (!m_displayWord.empty())
		{
			m_displayWord.pop_back();
			m_parent->m_console->PrintEditText(m_displayWord);
		}
	}
	else
	{
		const char* value = SDL_GetKeyName(ret);
		if (value != 0)
		{
			if (m_displayWord.size() < MAX_TALK)
			{
				std::string curLetter;

				if (ret == SDLK_SPACE)
				{
					m_displayWord += ' ';
					curLetter += ' ';
				}
				else
				{
					m_displayWord += value[0];
					curLetter += value[0];
				}

				m_parent->m_console->PrintEditText(m_displayWord);
			}
		}
	}
}

void U5World::HandleNameTalkInput()
{
	int ret = ProcessLetterImmediate();
	m_input->m_isValid = false;
	if (ret < 0)
	{
		return;
	}
	if (ret == SDLK_RETURN)
	{
		HandleNameTalkWord(m_displayWord);
		return;
	}
	else if (ret == SDLK_BACKSPACE)
	{
		if (!m_displayWord.empty())
		{
			m_displayWord.pop_back();
			m_parent->m_console->PrintEditText(m_displayWord);
		}
	}
	else
	{
		const char* value = SDL_GetKeyName(ret);
		if (value != 0)
		{
			if (m_displayWord.size() < MAX_TALK)
			{
				std::string curLetter;

				if (ret == SDLK_SPACE)
				{
					m_displayWord += ' ';
					curLetter += ' ';
				}
				else
				{
					m_displayWord += value[0];
					curLetter += value[0];
				}

				m_parent->m_console->PrintEditText(m_displayWord);
			}
		}
	}
}

