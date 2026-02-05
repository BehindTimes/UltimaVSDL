#include "U5Game.h"
#include "GameObject.h"
#include "UltimaVResource.h"
#include <memory>
#include "U5Input.h"
#include "U5Utils.h"
#include "SDL3Helper.h"
#include <SDL3/SDL_render.h>
#include "U5Enums.h"
#include "U5World.h"
#include "U5Dungeon.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include "U5Console.h"

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5Game::U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_window_width(0),
	m_window_height(0),
	m_location(GameLocation::World),
	m_old_location(GameLocation::World),
	m_curLocation(nullptr),
	m_map_level(0),
	m_map_type(0)
{
	m_world = std::make_unique<U5World>(sdl_helper, u5_resources);
	m_world->SetParent(this);
	m_dungeon = std::make_unique<U5Dungeon>(sdl_helper, u5_resources);
	m_console = std::make_unique<U5Console>(sdl_helper, u5_resources);
}

U5Game::~U5Game()
{
}

void U5Game::LoadData()
{
	m_input->SetInputType(InputType::ANY_KEY);
	m_input->SetKeyDelay(125);
	m_curLocation = m_world.get();
	m_curLocation->GetPos(m_old_position.first, m_old_position.second);
	LoadMap(-1); // Britannia
}

void U5Game::SetSDLData()
{
	GameObject::SetSDLData();
}

void U5Game::Render()
{
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();

	m_console->Render(m_tickElapse);
	
	if (nullptr != m_curLocation)
	{
		m_curLocation->Render();
	}

	float offsetX = 0;
	float offsetY = 0;

	if (nullptr != m_curLocation)
	{
		offsetX = m_curLocation->m_DisplayOffset.first;
		offsetY = m_curLocation->m_DisplayOffset.second;
		if (offsetY < -1)
		{
			offsetY = -1;
		}
		else if (offsetY > 1)
		{
			offsetY = 1;
		}
		if (offsetX < -1)
		{
			offsetX = -1;
		}
		else if (offsetX > 1)
		{
			offsetX = 1;
		}
		offsetX *= RENDER_TILE_WIDTH;
		offsetY *= RENDER_TILE_HEIGHT;
	}

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY]);
	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER], -RENDER_TILE_WIDTH + offsetX, -RENDER_TILE_HEIGHT + offsetY, 13.0f * RENDER_TILE_WIDTH, 13.0f * RENDER_TILE_HEIGHT);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY], HALF_TILE_WIDTH, HALF_TILE_HEIGHT, 11.0f * RENDER_TILE_WIDTH, 11.0f * RENDER_TILE_HEIGHT);
	m_sdl_helper->RenderPresent();
	m_sdl_helper->AnimateTiles(m_tickElapse);
}

void U5Game::ProcessEvents()
{
	if (nullptr != m_curLocation)
	{
		m_curLocation->ProcessEvents();
	}
}

void U5Game::GetElapsedTime()
{
	GameObject::GetElapsedTime();
	if (nullptr != m_curLocation)
	{
		m_curLocation->SetElapsedTime(m_tickElapse);
	}
}

void U5Game::ChangeLevel(int map_level)
{
	if (map_level < 0 || map_level >= 16)
	{
		return;
	}
	if (m_map_type < 0 || m_map_type > 4)
	{
		return;
	}
	m_map_level = map_level;

	MapTypes curMapType = static_cast<MapTypes>(m_map_type);
	switch (curMapType)
	{
	case MapTypes::Castle:
		m_currentMap = m_resources->m_data.castle_maps[m_map_level];
		break;
	case MapTypes::Dwelling:
		m_currentMap = m_resources->m_data.dwelling_maps[m_map_level];
		break;
	case MapTypes::Keep:
		m_currentMap = m_resources->m_data.keep_maps[m_map_level];
		break;
	case MapTypes::Town:
		m_currentMap = m_resources->m_data.town_maps[m_map_level];
		break;
	default:
		break;
	}
}

void U5Game::LoadMap(int map_num)
{
	const int MAX_MAPS = 40;
	const int MAX_TOWN_MAPS = 32;

	if (map_num >= MAX_MAPS)
	{
		return;
	}

	m_old_location = m_location;
	// Underworld or Britannia 
	if (map_num < 0)
	{
		if (map_num == -1) // Britannia
		{
			m_location = GameLocation::World;
			m_currentMap.clear();
			m_currentMap = m_resources->m_data.world_map;
			m_curLocation->SetPos(m_old_position.first, m_old_position.second);
		}
		else if (map_num == -2) // Underworld
		{
			m_location = GameLocation::Underworld;
		}
		return;
	}

	m_curLocation->GetPos(m_old_position.first, m_old_position.second);

	std::vector<int> Map_Types = {};
	m_map_type = map_num / 8;
	m_map_level = 0;
	if (map_num < MAX_TOWN_MAPS)
	{
		m_map_level = m_resources->m_data.location_z_index[map_num];
	}
	MapTypes curMapType = static_cast<MapTypes>(m_map_type);
	switch (curMapType)
	{
	case MapTypes::Castle:
		m_location = GameLocation::Town;
		m_currentMap.clear();
		m_currentMap = m_resources->m_data.castle_maps[m_map_level];
		m_curLocation->SetPos(15, 30);
		break;
	case MapTypes::Dwelling:
		m_location = GameLocation::Town;
		m_currentMap.clear();
		m_currentMap = m_resources->m_data.dwelling_maps[m_map_level];
		m_curLocation->SetPos(15, 30);
		break;
	case MapTypes::Keep:
		m_location = GameLocation::Town;
		m_currentMap.clear();
		m_currentMap = m_resources->m_data.keep_maps[m_map_level];
		m_curLocation->SetPos(15, 30);
		break;
	case MapTypes::Town:
		m_location = GameLocation::Town;
		m_currentMap.clear();
		m_currentMap = m_resources->m_data.town_maps[m_map_level];
		m_curLocation->SetPos(15, 30);
		break;
	default:
		break;
	}
}
