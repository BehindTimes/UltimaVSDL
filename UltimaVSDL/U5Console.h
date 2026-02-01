#pragma once

#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include <SDL3/SDL_stdinc.h>

class U5Console
{
public:
	U5Console(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Console();

	void Render(Uint64 tickElapse);
	void PrintText(std::string text);
private:
	void RenderCursor();

	SDL3Helper* m_sdl_helper;
	UltimaVResource* m_resources;
	Uint64 m_tickElapse;
	bool m_showPrompt;
	std::pair<int, int> m_cursorPos;
	int m_curCursor;
	Uint64 m_curCursorDelay;

	const Uint64 CURSOR_DELAY = 100;
};

