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

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5World::U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameBase(sdl_helper, u5_resources),
	m_location_type(GameLocation::World),
	m_parent(nullptr)
{
	m_xpos = 50;
	m_ypos = 50;
	m_DisplayOffset.first = 0;
	m_DisplayOffset.second = 0;

	m_smoothscroll = true;

	m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
}

U5World::~U5World()
{
}

void U5World::SetParent(U5Game* parent)
{
	m_parent = parent;
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
			if (m_parent->m_location == GameLocation::World)
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
			//curpos++;
		}
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

void U5World::ProcessNorth()
{
	int tempval;

	m_vec_pos.emplace_back();
	m_vec_pos.back().old_position.first = m_xpos;
	m_vec_pos.back().old_position.second = m_ypos;
	m_vec_pos.back().new_position.first = m_xpos;
	tempval = m_ypos;
	tempval--;
	if (m_parent->m_location == GameLocation::World)
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
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[4]);
		if (check_location == 0)
		{
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
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
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[5]);
		if (check_location == 0)
		{
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
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
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[6]);
		if (check_location == 0)
		{
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
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
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[7]);
		if (check_location == 0)
		{
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
	{
		tempval += static_cast<int>(m_parent->m_currentMap[0].size());
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	if (m_parent->m_location == GameLocation::World)
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
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
	{
		tempval += static_cast<int>(m_parent->m_currentMap[0].size());
		tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	tempval += static_cast<int>(m_parent->m_currentMap.size());
	tempval %= static_cast<int>(m_parent->m_currentMap.size());
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
			m_parent->m_console->PrintText("\n", true);
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
	if (m_parent->m_location == GameLocation::World)
	{
		tempval %= m_parent->m_currentMap[0].size();
	}
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	if (m_parent->m_location == GameLocation::World)
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
			m_parent->m_console->PrintText("\n", true);
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
	tempval %= static_cast<int>(m_parent->m_currentMap[0].size());
	m_vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	tempval += static_cast<int>(m_parent->m_currentMap.size());
	tempval %= static_cast<int>(m_parent->m_currentMap.size());
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
			m_parent->m_console->PrintText("\n", true);
		}
	}
}

void U5World::ProcessLeaveTown()
{
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[8]);
	m_process_key = std::bind(&U5World::HandleLeaveTown, this);
}

void U5World::HandleLeaveTown()
{
	int ret = ProcessYesNo();
	if (ret == 'Y')
	{
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[9]);
		if (m_parent->m_old_location == GameLocation::World)
		{
			m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[11]);
		}
		else if (m_parent->m_old_location == GameLocation::World)
		{
			m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[10]);
		}
		m_parent->m_console->PrintText("\n", true);
		m_parent->LoadMap(-1);
	}
	else if (ret == 'N')
	{
		m_process_key = std::bind(&U5World::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_1[12]);
		m_parent->m_console->PrintText("\n", true);
	}
}

int U5World::ProcessYesNo()
{
	m_input->m_isValid = true;
	SDL_Keycode curKey = m_input->GetKeyCode();
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

void U5World::ProcessAnyKeyHit()
{
	if (m_input->isAnyKeyHit())
	{
		m_input->m_isValid = true;
		if (m_vec_pos.size() > 0)
		{
			return;
		}
		SDL_Keycode curKey = m_input->GetKeyCode();
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

	bool allowMove;

	if (m_smoothscroll)
	{
		m_vec_pos.back().elapsed_time += m_tickElapse;
		allowMove = m_vec_pos.back().elapsed_time >= m_vec_pos.back().TURN_TIME;
	}
	else
	{
		allowMove = true;
	}
	
	if (allowMove)
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

void U5World::ProcessEnter()
{
	m_input->SetRequireAllKeysUp();

	if (m_location_type != GameLocation::World)
	{
		return;
	}
	std::pair<int, int> cur_location = {m_xpos, m_ypos};
	auto it = std::find(m_resources->m_data.location_info.begin(), m_resources->m_data.location_info.end(), cur_location);
	if (m_resources->m_data.location_info.end() != it)
	{
		size_t curMap = std::distance(m_resources->m_data.location_info.begin(), it);
		if (m_parent != nullptr)
		{
			m_parent->LoadMap(static_cast<int>(curMap));
		}
	}
	else
	{
	}
}
