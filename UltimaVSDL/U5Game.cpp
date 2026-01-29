#include "U5Game.h"
#include "GameObject.h"
#include "UltimaVResource.h"
#include <memory>
#include "U5Input.h"
#include "U5Utils.h"
#include "SDL3Helper.h"
#include <SDL3/SDL_render.h>
#include "U5Enums.h"
#include <SDL3/SDL_rect.h>

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5Game::U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_window_width(0),
	m_window_height(0),
	m_location(GameLocation::World)
{
}

U5Game::~U5Game()
{
}

void U5Game::LoadData()
{
	m_input->SetInputType(InputType::ANY_KEY);
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
	DrawBorder();
	m_sdl_helper->RenderPresent();
}

void U5Game::ProcessEvents()
{
}

void U5Game::DrawBorder()
{
	SDL_Texture* curTexture;
	SDL_FRect line;

	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][123];
	m_sdl_helper->RenderTextureAt(curTexture, 0, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][124];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][125];
	m_sdl_helper->RenderTextureAt(curTexture, 0, HALF_TILE_HEIGHT * 23, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);

	switch (m_location)
	{
	case GameLocation::Dungeon:
		break;
	case GameLocation::World:
	{
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
	}
		break;
	default:
		break;
	}

	m_sdl_helper->DrawTileRect(24, 7, 15, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(24, 10, 15, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(39, 1, 1, 10, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(0, 1, 1, 22, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(23, 1, 1, 23, 1, m_resources->m_render_mode);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, LINE_THICKNESS, (22 * HALF_TILE_WIDTH) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT * 23, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 23, HALF_TILE_HEIGHT - LINE_THICKNESS, LINE_THICKNESS, (22 * HALF_TILE_WIDTH) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 17, HALF_TILE_HEIGHT * 23, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24, HALF_TILE_HEIGHT - LINE_THICKNESS, (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (7 * HALF_TILE_HEIGHT), (15 * HALF_TILE_WIDTH) + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24, (8 * HALF_TILE_HEIGHT - LINE_THICKNESS), (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (10 * HALF_TILE_HEIGHT), (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (11 * HALF_TILE_HEIGHT - LINE_THICKNESS), (16 * HALF_TILE_WIDTH), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (11 * HALF_TILE_HEIGHT - LINE_THICKNESS), LINE_THICKNESS, (13 * HALF_TILE_HEIGHT) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (8 * HALF_TILE_HEIGHT - LINE_THICKNESS), LINE_THICKNESS, (2 * HALF_TILE_HEIGHT) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (1 * HALF_TILE_HEIGHT - LINE_THICKNESS), LINE_THICKNESS, (6 * HALF_TILE_HEIGHT) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 39, (8 * HALF_TILE_HEIGHT - LINE_THICKNESS), LINE_THICKNESS, (2 * HALF_TILE_HEIGHT) + 2 * LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 39, (1 * HALF_TILE_HEIGHT - LINE_THICKNESS), LINE_THICKNESS, (6 * HALF_TILE_HEIGHT) + LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}
