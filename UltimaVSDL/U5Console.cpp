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
#include "GameOptions.h"

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;
extern std::unique_ptr<GameOptions> m_options;

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
	m_cursorPosY(12),
	m_scroll(false),
	m_scrollOffset(0),
	m_startLine(0),
	m_hasPrompt(true),
	m_smoothscroll(m_options->m_console_smooth_scroll),
	m_cached_startLine(0),
	m_is_editing(false)
{
	ShowPrompt();
}

U5Console::~U5Console()
{
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

	if (startElem >= CONSOLE_SIZE)
	{
		ret.emplace_back(std::string("\n"));
		max_len = CONSOLE_SIZE;
	}

	if (pretty_print)
	{
		m_utilities->ReplaceStringInPlace(text, "  ", " ");
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
			else
			{
				ret.emplace_back(text.substr(0, CONSOLE_SIZE));
				ret.emplace_back(std::string("\n"));
				text.erase(0, CONSOLE_SIZE);
			}
		}
		max_len = CONSOLE_SIZE;
	}

	return ret;
}

void U5Console::SetCursorStartPosX(int startPos)
{
	m_cursorPosX = startPos;
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

void U5Console::UpdateCursor()
{
	const int CONSOLE_SIZE = 16;
	if (m_cursorPosX >= CONSOLE_SIZE)
	{
		if (m_cursorPosY >= 12)
		{
			m_buffer_strings.push_back({ 0, "\n" });
		}
	}
	if (!m_scroll)
	{
		CheckText();
	}
}

void U5Console::BackspaceCursor()
{
	const int CONSOLE_SIZE = 16;
	m_cursorPosX--;
	if (m_cursorPosX < 0)
	{
		m_cursorPosX = CONSOLE_SIZE - 1;
		m_cursorPosY--;
	}
}

void U5Console::StartLineEdit()
{
	m_cachedText.clear();
	m_cached_startLine = m_curLine;
	m_is_editing = true;
}

void U5Console::EndLineEdit()
{
	m_cached_startLine = m_curLine;
	m_is_editing = false;
	m_cursorPosY = 12;
}

bool U5Console::LineWasIncremented() const
{
	return m_cached_startLine == m_curLine;
}

// TO DO: Before text entry, we can grab the current m_curLine.
// This way, we can enter multiline text into the console, rather
// than just the hack
void U5Console::PrintEditText(std::string text)
{
	/*if (m_scroll)
	{
		return;
	}*/
	m_cachedText = text;

	int scroll_offset = m_scroll ? 1 : 0;
	int temp_curline = m_curLine + scroll_offset;
	const int CONSOLE_SIZE = 16;

	std::string str1 = text;
	std::string str2;
	int second_line = (m_cached_startLine + 1) % NUM_BUF_LINES;

	int cursorline = 12;

	if (text.length() >= CONSOLE_SIZE - 1)
	{
		if (m_cached_startLine == temp_curline)
		{
			PrintText("\n");
		}
		str1 = text.substr(0, CONSOLE_SIZE - 1);
		str2 = text.substr(CONSOLE_SIZE - 1);
		m_cursorPosY = second_line;
	}
	else
	{
		if (m_cached_startLine != temp_curline)
		{
			cursorline = 11;
		}
	}
	
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
	m_sdl_helper->DrawTileRect(1, m_cached_startLine, CONSOLE_SIZE, 1);
	m_sdl_helper->DrawTiledText(str1, 1, m_cached_startLine);
	if (m_cached_startLine != temp_curline)
	{
		m_sdl_helper->DrawTileRect(0, second_line, CONSOLE_SIZE, 1);
		m_sdl_helper->DrawTiledText(str2, 0, second_line);
	}
	m_cursorPosX = static_cast<int>(str1.size() + 1);
	if (str1.size() >= CONSOLE_SIZE - 1)
	{
		m_cursorPosX = static_cast<int>(str2.size());
	}
	m_cursorPosY = cursorline;
	m_sdl_helper->SetRenderTarget(nullptr);
}

/*void U5Console::PrintEditText(std::string text, bool allowNewLine)
{
	if (m_scroll)
	{
		return;
	}
	const int CONSOLE_SIZE = 16;
	int cursorline = 12;

	std::string str1 = text;
	std::string str2;

	int drawline = m_curLine + NUM_BUF_LINES;
	if (!allowNewLine)
	{
		drawline--;
		cursorline--;
	}
	drawline %= NUM_BUF_LINES;

	if (text.length() >= CONSOLE_SIZE - 1)
	{
		str1 = text.substr(0, CONSOLE_SIZE - 1);
		str2 = text.substr(CONSOLE_SIZE - 1);
		cursorline++;
		if (allowNewLine)
		{
			m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
			m_sdl_helper->DrawTiledText(str1, 1, drawline);
			m_sdl_helper->SetRenderTarget(nullptr);
			return;
		}
	}

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
	m_sdl_helper->DrawTileRect(1, drawline, CONSOLE_SIZE, 1);
	if (m_curLine != drawline)
	{
		m_sdl_helper->DrawTileRect(0, m_curLine, CONSOLE_SIZE, 1);
	}
	m_sdl_helper->DrawTiledText(str1, 1, drawline);
	if (!str2.empty())
	{
		m_sdl_helper->DrawTiledText(str2, 0, m_curLine);
	}
	m_cursorPosX = static_cast<int>(str1.size() + 1);
	if (str1.size() >= CONSOLE_SIZE - 1)
	{
		m_cursorPosX = static_cast<int>(str2.size());
	}
	m_cursorPosY = cursorline;
	m_sdl_helper->SetRenderTarget(nullptr);
}*/

void U5Console::PrintText(std::string text, bool showElem, bool partial, bool pretty_print)
{
	if (showElem)
	{
		m_cursorPosY = 12;
	}
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

bool U5Console::IsReady()
{
	if (m_scroll)
	{
		return false;
	}
	if (!m_buffer_strings.empty())
	{
		return false;
	}
	return true;
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

		if (m_is_editing)
		{
			PrintEditText(m_cachedText);
		}
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
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>(5 + m_curCursor)], m_cursorPosX, m_cursorPosY);
}

void U5Console::ShowPrompt()
{
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_CONSOLE_BUFFER]);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 0, m_curLine);
	m_sdl_helper->SetRenderTarget(nullptr);
}

void U5Console::SetCursorVisible(bool isVisible)
{
	m_showPrompt = isVisible;
}
