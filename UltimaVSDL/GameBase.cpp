#include "GameBase.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include "ColorData.h"

GameBase::GameBase(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_xpos(0),
	m_ypos(0),
	m_smoothscroll(false),
	m_dir('N')
{
}

GameBase::~GameBase()
{
}

void GameBase::DrawBorder()
{
	SDL_Texture* curTexture;
	SDL_FRect line;
	unsigned char color_array[3];

	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][123];
	m_sdl_helper->RenderTextureAt(curTexture, 0, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][124];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][125];
	m_sdl_helper->RenderTextureAt(curTexture, 0, HALF_TILE_HEIGHT * 23, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);

	/*m_sdl_helper->DrawTileRect(24, 7, 15, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(24, 10, 15, 1, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(39, 1, 1, 10, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(0, 1, 1, 22, 1, m_resources->m_render_mode);
	m_sdl_helper->DrawTileRect(23, 1, 1, 23, 1, m_resources->m_render_mode);*/

	if (m_resources->m_render_mode == RenderMode::CGA)
	{
		std::memcpy(color_array, cga_table[1], sizeof(color_array));	
	}
	else
	{
		std::memcpy(color_array, ega_table[1], sizeof(color_array));
	}

	m_sdl_helper->DrawTileRect(24, 7, 15, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(24, 10, 15, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(39, 1, 1, 10, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(0, 1, 1, 22, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(23, 1, 1, 23, color_array[0], color_array[1], color_array[2]);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, LINE_THICKNESS, (22 * HALF_TILE_WIDTH) + 2 * LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 23, HALF_TILE_HEIGHT - LINE_THICKNESS, LINE_THICKNESS, (22 * HALF_TILE_WIDTH) + 2 * LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24, HALF_TILE_HEIGHT - LINE_THICKNESS, (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (7 * HALF_TILE_HEIGHT), (15 * HALF_TILE_WIDTH) + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24, (8 * HALF_TILE_HEIGHT - LINE_THICKNESS), (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (10 * HALF_TILE_HEIGHT), (15 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 24 - LINE_THICKNESS, (11 * HALF_TILE_HEIGHT - LINE_THICKNESS), (16 * HALF_TILE_WIDTH) + LINE_THICKNESS, LINE_THICKNESS };
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

void GameBase::SetPos(int x, int y)
{
	m_xpos = x;
	m_ypos = y;
}

void GameBase::GetPos(int &x, int &y) const
{
	x = m_xpos;
	y = m_ypos;
}

void GameBase::SetDir(int dir)
{
	m_dir = dir;
}

void GameBase::GetDir(int& dir) const
{
	dir = m_dir;
}
