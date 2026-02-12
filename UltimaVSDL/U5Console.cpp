#include "U5Console.h"
#include "U5Tile.h"
#include "UltimaVResource.h"
#include <SDL3/SDL_render.h>
#include "SDL3Helper.h"
#include "U5Enums.h"
#include <string>
#include <SDL3/SDL_stdinc.h>
#include <memory>
#include "U5Utils.h"
#include "U5Input.h"
#include <iostream>
#include <vector>
#include <deque>
#include <utility>

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5Console::U5Console(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources),
	m_tickElapse(0),
	m_showPrompt(true),
	m_curCursor(0),
	m_curCursorRenderDelay(0),
	m_curScrollDelay(0),
	m_curLine(12),
	m_cursorPosX(1),
	m_scroll(false),
	m_scrollOffset(0),
	m_startLine(0),
	m_blockPrompt(false),
	m_hasPrompt(true),
	m_smoothscroll(true)
{
	ShowPrompt();
}

U5Console::~U5Console()
{
}

void U5Console::BlockPrompt(bool block)
{
	m_blockPrompt = block;
}

void U5Console::NewPrompt()
{
	PrintText("\n", true);
}

void U5Console::Render(Uint64 tickElapse)
{
	m_tickElapse = tickElapse;
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();

	if (m_scroll)
	{
		ProcessScroll();
		m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE]);
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER], 0, static_cast<float>(-m_startLine * HALF_TILE_HEIGHT) - m_scrollOffset, 16.0f * HALF_TILE_WIDTH, static_cast<float>(NUM_BUF_LINES * HALF_TILE_HEIGHT));
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER], 0, static_cast<float>((14 - m_startLine) * HALF_TILE_HEIGHT) - m_scrollOffset, 16.0f * HALF_TILE_WIDTH, static_cast<float>(NUM_BUF_LINES * HALF_TILE_HEIGHT));
	}
	else
	{
		CheckText();
		m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE]);
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER], 0, static_cast<float>(-m_startLine * HALF_TILE_HEIGHT), 16.0f * HALF_TILE_WIDTH, static_cast<float>(NUM_BUF_LINES * HALF_TILE_HEIGHT));
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER], 0, static_cast<float>((14 - m_startLine) * HALF_TILE_HEIGHT), 16.0f * HALF_TILE_WIDTH, static_cast<float>(NUM_BUF_LINES * HALF_TILE_HEIGHT));

		if (m_showPrompt)
		{
			RenderCursor();
		}
	}
	
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_TargetTextures[TTV_CONSOLE], 24 * HALF_TILE_WIDTH, 11 * HALF_TILE_HEIGHT, 16 * HALF_TILE_WIDTH, static_cast<float>(NUM_BUF_LINES - 1) * HALF_TILE_HEIGHT);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}

void U5Console::ClearLine()
{
	int tempLine = m_curLine + 2;
	tempLine %= NUM_BUF_LINES;
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
	m_sdl_helper->DrawTileRect(0, tempLine, 16, 1);
	m_sdl_helper->SetRenderTarget(nullptr);
}

bool U5Console::CheckText()
{
	if (m_buffer_strings.empty())
	{
		return false;
	}
	
	auto& curPair = m_buffer_strings.front();
	std::string curLine = curPair.second;
	if (curPair.first == 1)
	{
		m_hasPrompt = true;
	}
	else
	{
		m_hasPrompt = false;
	}
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);

	m_scroll = curLine == std::string("\n");
	if (!m_scroll)
	{
		m_sdl_helper->DrawTiledText(curLine, m_cursorPosX, m_curLine);
		m_cursorPosX += static_cast<int>(curLine.size());
	}
	else
	{
		m_cursorPosX = m_hasPrompt ? 1 : 0;
	}
	m_buffer_strings.pop_front();

	m_sdl_helper->SetRenderTarget(nullptr);
	
	return m_scroll;
}

std::vector<std::string> U5Console::FormatText(std::string text, int startElem, bool pretty_print)
{
	const int CONSOLE_SIZE = 16;
	size_t max_len = static_cast<size_t>(CONSOLE_SIZE - startElem);
	std::vector<std::string> ret;

	if (pretty_print == false)
	{
		int j = 9;
	}
	
	bool valid = true;
	while (valid)
	{
		if (text.size() <= max_len)
		{
			if (!text.empty())
			{
				ret.push_back(text);
			}
			return ret;
		}

		bool found = false;
		int found_pos = -1;
		if (pretty_print)
		{
			for (size_t curIndex = max_len; curIndex > 0; curIndex--)
			{
				if (text[curIndex] == ' ')
				{
					found = true;
					found_pos = static_cast<int>(curIndex);
					break;
				}
			}
		}
		if (found)
		{
			ret.emplace_back(text.substr(0, found_pos));
			ret.emplace_back(std::string("\n"));
			text.erase(0, static_cast<size_t>(found_pos + 1));
		}
		else
		{
			if (startElem > 1)
			{
				ret.emplace_back(std::string("\n"));
			}
			ret.emplace_back(text.substr(0, CONSOLE_SIZE));
			ret.emplace_back(std::string("\n"));
			text.erase(0, CONSOLE_SIZE);
		}
		max_len = CONSOLE_SIZE;
	}

	return ret;
}

int U5Console::GetCursorStartPos()
{
	if (m_buffer_strings.empty())
	{
		return m_cursorPosX;
	}
	int curpos = 0;
	for (std::deque<std::pair<int,std::string>>::reverse_iterator it = m_buffer_strings.rbegin(); it != m_buffer_strings.rend(); ++it)
	{
		if ((*it).second == std::string("\n"))
		{
			return curpos;
		}
		curpos += static_cast<int>((*it).second.size());
	}

	return curpos + 1;
}

void U5Console::PrintText(std::string text, bool showElem, bool partial, bool pretty_print)
{
	auto strVals = m_utilities->splitString(text, '\n', true);

	if (strVals.size() > 0)
	{
		bool first = true;
		for (auto& elem : strVals)
		{
			if (elem == std::string("\n"))
			{
				m_buffer_strings.push_back({ showElem ? 1 : 0, elem });
				showElem = false;
			}
			else
			{
				auto tempVals = FormatText(elem, first ? GetCursorStartPos() : 0, pretty_print);
				first = false;
				for (auto& elem1 : tempVals)
				{
					m_buffer_strings.push_back({ showElem ? 1 : 0, elem1 });
					showElem = false;
				}
			}	
		}
		if (!m_scroll && !partial)
		{
			CheckText();
		}
	}
}

void U5Console::ProcessScroll()
{
	m_curScrollDelay += m_tickElapse;
	if (m_curScrollDelay >= SCROLL_DELAY)
	{
		ClearLine();
		m_scrollOffset = 0;
		m_scroll = false;
		m_curScrollDelay = 0;
		m_startLine++;
		m_curLine++;
		m_curLine %= NUM_BUF_LINES;
		m_startLine %= NUM_BUF_LINES;
		if (m_hasPrompt)
		{
			ShowPrompt();
			m_cursorPosX = 1;
		}
		else
		{
			m_cursorPosX = 0;
		}
		CheckText();
	}
	else
	{
		if (m_smoothscroll)
		{
			float ratio = static_cast<float>(m_curScrollDelay) / SCROLL_DELAY;
			m_scrollOffset = HALF_TILE_HEIGHT * ratio;
		}
	}
}

void U5Console::RenderCursor()
{
	if (m_input->IsAnyKeyDown() && m_input->m_isValid)
	{
		return;
	}
	if (m_scroll)
	{
		return;
	}
	if (!m_buffer_strings.empty())
	{
		return;

	}
	m_curCursorRenderDelay += m_tickElapse;
	if (m_curCursorRenderDelay >= CURSOR_RENDER_DELAY)
	{
		const int NUM_CURSOR = 4;
		m_curCursor++;
		m_curCursor %= NUM_CURSOR;
		m_curCursorRenderDelay %= CURSOR_RENDER_DELAY;
	}
	int temppos = m_startLine + 12;
	temppos %= 14;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>(5 + m_curCursor)], m_cursorPosX, 12);
}

void U5Console::ShowPrompt()
{
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 0, m_curLine);
	m_sdl_helper->SetRenderTarget(nullptr);
}
