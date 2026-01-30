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

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5World::U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameBase(sdl_helper, u5_resources)
{
	m_xpos = 50;
	m_ypos = 50;
}

U5World::~U5World()
{
}

void U5World::Render()
{
	DrawBorder();
	// We're going to render to the buffer, which gives us a 1 tile buffer on each side
	// This way, we can enable smooth scrolling in the future if we want
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();

	int mapX = m_xpos - 6;
	int mapY = m_ypos - 6;
	for (int ypos = 0; ypos < 13; ypos++)
	{
		for (int xpos = 0; xpos < 13; xpos++)
		{
			int curX = (mapX + xpos + 250) % 256;
			int curY = (mapY + ypos + 250) % 256;
			unsigned char curpos = m_resources->m_data.world_map[curX][curY];
			SDL_Texture* curTexture = m_sdl_helper->m_TileTextures[curpos].GetTexture();
			m_sdl_helper->DrawTileTexture(curTexture, xpos, ypos);
			curpos++;
		}
	}
	
}

void U5World::ProcessEvents()
{
	if (m_input->isAnyKeyHit())
	{
		SDL_Keycode curKey = m_input->GetKeyCode();
		switch (curKey)
		{
		case SDLK_UP:
			m_ypos--;
			m_ypos += 256;
			m_ypos %= 256;
			break;
		case SDLK_DOWN:
			m_ypos++;
			m_ypos %= 256;
			break;
		case SDLK_LEFT:
			m_xpos--;
			m_xpos += 256;
			m_xpos %= 256;
			break;
		case SDLK_RIGHT:
			m_xpos++;
			m_xpos %= 256;
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
}
