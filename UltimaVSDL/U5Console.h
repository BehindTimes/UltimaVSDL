#pragma once

#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include <SDL3/SDL_stdinc.h>
#include <deque>
#include <string>
#include <utility>
#include <vector>

class U5Console
{
public:
	U5Console(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Console();

	void Render(Uint64 tickElapse);
	void PrintText(std::string text, bool showElem = false, bool partial = false, bool pretty_print = true);
	void NewPrompt();
	void ShowPrompt();
	void BlockPrompt(bool block);
private:
	void RenderCursor();
	void ClearLine();
	void ProcessScroll();
	bool CheckText();
	int GetCursorStartPos();
	std::vector<std::string> FormatText(std::string text, int startElem = 0, bool pretty_print = true);

	SDL3Helper* m_sdl_helper;
	UltimaVResource* m_resources;
	Uint64 m_tickElapse;
	bool m_showPrompt;
	int m_cursorPosX;
	int m_cursorPosY;
	int m_curCursor;
	Uint64 m_curCursorRenderDelay;
	Uint64 m_curScrollDelay;
	int m_curLine;
	bool m_scroll;
	bool m_blockPrompt;
	bool m_hasPrompt;
	bool m_smoothscroll;

	float m_scrollOffset;
	int m_startLine;

	const Uint64 CURSOR_RENDER_DELAY = 100;
	const Uint64 SCROLL_DELAY = 60;
	const int NUM_BUF_LINES = 14;
	std::deque<std::pair<int, std::string>> m_buffer_strings;
};

