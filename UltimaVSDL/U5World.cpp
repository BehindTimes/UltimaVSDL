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
	if (!vec_pos.empty())
	{
		ProcessScroll();
	}
	// We're going to render to the buffer, which gives us a 1 tile buffer on each side
	// This way, we can enable smooth scrolling in the future if we want
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();

	//unsigned char curpos = 0;

	int mapX = m_xpos - 6;
	int mapY = m_ypos - 6;
	for (int ypos = 0; ypos < 13; ypos++)
	{
		for (int xpos = 0; xpos < 13; xpos++)
		{
			int curX = (mapX + xpos + 256) % 256;
			int curY = (mapY + ypos + 256) % 256;
			unsigned char curpos = m_resources->m_data.world_map[curX][curY];
			SDL_Texture* curTexture = m_sdl_helper->m_TileTextures[curpos].GetTexture();
			m_sdl_helper->DrawTileTexture(curTexture, xpos, ypos);
			//curpos++;
		}
	}
}

int U5World::checkValidLocation(const PositionData& pos_info)
{
	return 0;
}

void U5World::ProcessNorth()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	vec_pos.back().new_position.first = m_xpos;
	tempval = m_ypos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessSouth()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	vec_pos.back().new_position.first = m_xpos;
	tempval = m_ypos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessEast()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	vec_pos.back().new_position.second = m_ypos;
	tempval = m_xpos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessWest()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	vec_pos.back().new_position.second = m_ypos;
	tempval = m_xpos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessNorthEast()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessNorthWest()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessSouthEast()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessSouthWest()
{
	int tempval;

	vec_pos.emplace_back();
	vec_pos.back().old_position.first = m_xpos;
	vec_pos.back().old_position.second = m_ypos;
	tempval = m_ypos;
	tempval++;
	tempval %= 256;
	vec_pos.back().new_position.second = tempval;
	tempval = m_xpos;
	tempval--;
	tempval += 256;
	tempval %= 256;
	vec_pos.back().new_position.first = tempval;
	if (0 != checkValidLocation(vec_pos.back()))
	{
		vec_pos.pop_back();
	}
	else
	{
		m_input->EnableInput(false);
	}
}

void U5World::ProcessEvents()
{
	if (m_input->isAnyKeyHit())
	{	
		if (vec_pos.size() > 0)
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
			break;
		}
	}
}

void U5World::DrawBorder()
{
	SDL_FRect line;

	m_sdl_helper->DrawTileRect(1, 0, 6, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(1, 23, 6, 1, 1, m_resources->m_render_mode);

	m_sdl_helper->DrawTileRect(17, 0, 22, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(17, 23, 6, 1, 1, m_resources->m_render_mode);

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
	if (vec_pos.empty())
	{
		return;
	}
	bool allowMove;

	if (m_smoothscroll)
	{
		vec_pos.back().elapsed_time += m_tickElapse;
		allowMove = vec_pos.back().elapsed_time >= vec_pos.back().TURN_TIME;
	}
	else
	{
		allowMove = true;
	}
	
	if (allowMove)
	{
		m_xpos = vec_pos.back().new_position.first;
		m_ypos = vec_pos.back().new_position.second;
		vec_pos.clear();
		m_DisplayOffset.first = 0;
		m_DisplayOffset.second = 0;
		m_input->EnableInput(true);
	}
	else
	{
		float ratio = static_cast<float>(vec_pos.back().elapsed_time) / vec_pos.back().TURN_TIME;
		float temppos1 = static_cast<float>(vec_pos.back().old_position.first);
		float temppos2 = static_cast<float>(vec_pos.back().new_position.first);

		if (vec_pos.back().old_position.first == 0 && vec_pos.back().new_position.first == 255)
		{
			temppos2 = -1;
		}
		else if (vec_pos.back().old_position.first == 255 && vec_pos.back().new_position.first == 0)
		{
			temppos2 = 256;
		}
		m_DisplayOffset.first = (temppos1 - temppos2) * ratio;

		temppos1 = static_cast<float>(vec_pos.back().old_position.second);
		temppos2 = static_cast<float>(vec_pos.back().new_position.second);
		if (vec_pos.back().old_position.second == 0 && vec_pos.back().new_position.second == 255)
		{
			temppos2 = -1;
		}
		else if (vec_pos.back().old_position.second == 255 && vec_pos.back().new_position.second == 0)
		{
			temppos2 = 256;
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
