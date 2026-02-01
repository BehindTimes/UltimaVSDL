#include "U5Console.h"
#include "U5Tile.h"
#include "UltimaVResource.h"
#include <SDL3/SDL_render.h>
#include "SDL3Helper.h"
#include "U5Enums.h"
#include <string>
#include <SDL3/SDL_stdinc.h>

U5Console::U5Console(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources),
	m_tickElapse(0),
	m_showPrompt(true),
	m_curCursor(0),
	m_curCursorDelay(0)
{
	m_cursorPos.first = 1;
	m_cursorPos.second = 12;
}

U5Console::~U5Console()
{
}

void U5Console::Render(Uint64 tickElapse)
{
	m_tickElapse = tickElapse;
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();
	if (m_showPrompt)
	{
		RenderCursor();
	}
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE], 24 * HALF_TILE_WIDTH, 11 * HALF_TILE_HEIGHT, 16 * HALF_TILE_WIDTH, 13 * HALF_TILE_HEIGHT);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}

void U5Console::PrintText(std::string text)
{
}

void U5Console::RenderCursor()
{
	m_curCursorDelay += m_tickElapse;
	if (m_curCursorDelay >= CURSOR_DELAY)
	{
		const int NUM_CURSOR = 4;
		m_curCursor++;
		m_curCursor %= NUM_CURSOR;
		m_curCursorDelay %= CURSOR_DELAY;
	}
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>(5 + m_curCursor)], m_cursorPos.first, m_cursorPos.second);
}