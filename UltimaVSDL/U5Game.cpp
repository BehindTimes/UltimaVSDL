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

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5Game::U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_window_width(0),
	m_window_height(0),
	m_location(GameLocation::World),
	m_curLocation(nullptr)
{
	m_world = std::make_unique<U5World>(sdl_helper, u5_resources);
	m_dungeon = std::make_unique<U5Dungeon>(sdl_helper, u5_resources);
}

U5Game::~U5Game()
{
}

void U5Game::LoadData()
{
	m_input->SetInputType(InputType::ANY_KEY);
	m_input->SetKeyDelay(125);
	m_curLocation = m_world.get();
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

