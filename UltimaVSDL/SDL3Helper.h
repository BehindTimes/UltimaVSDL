#pragma once

#include <SDL3/SDL.h>

#include "UltimaVResource.h"

class SDL3Helper
{
public:
	explicit SDL3Helper();

	int Intialize();
	void Cleanup();
	void Render();
	void Poll();
	void LoadImageData(UltimaVResource *u5_resources);

	bool m_quit;
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	SDL_Event m_event;

	SDL_Texture* m_Texture;
};

