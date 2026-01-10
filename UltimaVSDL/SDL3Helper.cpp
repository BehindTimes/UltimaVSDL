#include <cstring>

#include "SDL3Helper.h"
#include "ColorData.h"

SDL3Helper::SDL3Helper() :
	m_window(nullptr),
	m_renderer(nullptr),
	m_quit(false),
	m_Texture(nullptr),
	m_event(NULL)
{
}

int SDL3Helper::Intialize()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if (!SDL_CreateWindowAndRenderer("Ultima V - SDL", 1280, 800, SDL_WINDOW_OPENGL /*| SDL_WINDOW_HIGH_PIXEL_DENSITY*/, &m_window, &m_renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}
	return 0;
}

void SDL3Helper::Cleanup()
{
	if (m_Texture)
	{
		SDL_DestroyTexture(m_Texture);
		m_Texture = nullptr;
	}

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void SDL3Helper::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_SetRenderTarget(m_renderer, NULL);
	SDL_RenderClear(m_renderer);

	if (m_Texture)
	{
		SDL_RenderTexture(m_renderer, m_Texture, NULL, NULL);
	}

	SDL_RenderPresent(m_renderer);
}

void SDL3Helper::Poll()
{
	SDL_PollEvent(&m_event);
	switch (m_event.type)
	{
	case SDL_EVENT_QUIT:
		m_quit = true;
		break;
	case SDL_EVENT_KEY_DOWN:
		m_quit = true;
		break;
	}
}

void SDL3Helper::LoadImageData(UltimaVResource *u5_resources)
{
	auto curData = u5_resources->getBitData();
	m_Texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, (int)curData.width, (int)curData.height);
	SDL_SetTextureScaleMode(m_Texture, SDL_SCALEMODE_NEAREST);

	std::vector<unsigned char> canvas;
	size_t canvasSize = static_cast<size_t>(curData.width) * static_cast<size_t>(curData.height) * 4;
	canvas.resize(canvasSize);
	std::fill(canvas.begin(), canvas.end(), static_cast<unsigned char>(0));

	for (size_t y = 0; y < curData.height; y++)
	{
		for (size_t x = 0; x < curData.width; x++)
		{
			unsigned char curByte = curData.pixel_data[y * curData.width + x];
			unsigned char colorArray[3];
			if (curByte == 0)
			{
				std::memcpy(colorArray, ega_table[0], sizeof(colorArray));
			}
			else
			{
				std::memcpy(colorArray, ega_table[15], sizeof(colorArray));
			}
			canvas[(y * curData.width * 4 + x * 4) + 0] = 0xFF;
			canvas[(y * curData.width * 4 + x * 4) + 1] = colorArray[0];
			canvas[(y * curData.width * 4 + x * 4) + 2] = colorArray[1];
			canvas[(y * curData.width * 4 + x * 4) + 3] = colorArray[2];
		}
	}
	SDL_GetTextureProperties(m_Texture);
	SDL_UpdateTexture(m_Texture, NULL, canvas.data(), (int)curData.width * 4);
}
