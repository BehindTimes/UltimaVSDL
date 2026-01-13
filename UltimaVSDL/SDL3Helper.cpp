#include <cstring>

#include "SDL3Helper.h"
#include "ColorData.h"
#include <algorithm>
#include <vector>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include "UltimaVResource.h"

SDL3Helper::SDL3Helper() :
	m_window(nullptr),
	m_renderer(nullptr),
	m_quit(false),
	m_event(NULL),
	m_curTick(0),
	m_PathFileTexture(nullptr),
	m_LogoFadeTexture(nullptr),
	m_WoDFadeTexture(nullptr)
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

	//SDL_SetWindowFullscreen(m_window, true);
	return 0;
}

void SDL3Helper::Cleanup()
{
	for (auto& curBit : m_BitFileTextures)
	{
		for (auto& curTexture : curBit)
		{
			if (curTexture)
			{
				SDL_DestroyTexture(curTexture);
			}
		}
	}
	m_BitFileTextures.clear();

	for (auto& curImage : m_Image16FileTextures)
	{
		for (auto& curTexture : curImage)
		{
			if (curTexture)
			{
				SDL_DestroyTexture(curTexture);
			}
		}
	}
	m_Image16FileTextures.clear();

	if (m_LogoFadeTexture)
	{
		SDL_DestroyTexture(m_LogoFadeTexture);
		m_LogoFadeTexture = nullptr;
	}

	if (m_Flame1FadeTexture)
	{
		SDL_DestroyTexture(m_Flame1FadeTexture);
		m_Flame1FadeTexture = nullptr;
	}

	if (m_WoDFadeTexture)
	{
		SDL_DestroyTexture(m_WoDFadeTexture);
		m_WoDFadeTexture = nullptr;
	}

	if (m_PathFileTexture)
	{
		SDL_DestroyTexture(m_PathFileTexture);
		m_PathFileTexture = nullptr;
	}

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void SDL3Helper::GetScreenDimensions(int &width, int &height)
{
	SDL_GetWindowSize(m_window , &width, &height);
}

void SDL3Helper::ClearScreen()
{
	SDL_RenderClear(m_renderer);
}

void SDL3Helper::RenderTextureAt(SDL_Texture* texture, float x, float y, float width, float height)
{
	SDL_FRect toRect{};
	toRect.x = static_cast<float>(x);
	toRect.y = static_cast<float>(y);
	toRect.w = static_cast<float>(width);
	toRect.h = static_cast<float>(height);

	if (texture)
	{
		SDL_RenderTexture(m_renderer, texture, NULL, &toRect);
	}
}

void SDL3Helper::RenderFlipTextureAt(SDL_Texture* texture, float x, float y, float width, float height, bool flip)
{
	SDL_FRect toRect{};
	toRect.x = static_cast<float>(x);
	toRect.y = static_cast<float>(y);
	toRect.w = static_cast<float>(width);
	toRect.h = static_cast<float>(height);

	if (texture)
	{
		SDL_FlipMode flipmode = SDL_FLIP_NONE;
		if (flip)
		{
			flipmode = SDL_FLIP_VERTICAL;
		}

		SDL_RenderTextureRotated(m_renderer, texture, NULL, &toRect, 0, NULL, flipmode);
	}
}

void SDL3Helper::RenderPresent()
{
	SDL_RenderPresent(m_renderer);
}

void SDL3Helper::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_SetRenderTarget(m_renderer, NULL);
	SDL_RenderClear(m_renderer);

	if (m_BitFileTextures[2][0])
	{
		SDL_RenderTexture(m_renderer, m_BitFileTextures[2][0], NULL, NULL);
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

void SDL3Helper::CreateTextureFromMemory(SDL_Texture *&texture, const U5ImageData &curData)
{
	texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
		static_cast<int>(curData.width), static_cast<int>(curData.height));
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	std::vector<unsigned char> canvas;
	size_t canvasSize = static_cast<size_t>(curData.width) * static_cast<size_t>(curData.height) * 4;
	canvas.resize(canvasSize);
	std::fill(canvas.begin(), canvas.end(), static_cast<unsigned char>(0));

	for (size_t y = 0; y < curData.height; y++)
	{
		for (size_t x = 0; x < curData.width; x++)
		{
			unsigned char curByte = curData.pixel_data[y * curData.width + x];
			unsigned char colorArray[3] = {};
			if (curData.mode == 8)
			{
				if (curByte == 0)
				{
					std::memcpy(colorArray, ega_table[0], sizeof(colorArray));
				}
				else
				{
					std::memcpy(colorArray, ega_table[15], sizeof(colorArray));
				}
			}
			else if (curData.mode == 4)
			{
				if (curByte < 4)
				{
					std::memcpy(colorArray, cga_table[curByte], sizeof(colorArray));
				}
			}
			else
			{
				if (curByte < 16)
				{
					std::memcpy(colorArray, ega_table[curByte], sizeof(colorArray));
				}
			}
			
			// ABGR
			canvas[(y * curData.width * 4 + x * 4) + 0] = 0xFF;
			canvas[(y * curData.width * 4 + x * 4) + 1] = colorArray[2];
			canvas[(y * curData.width * 4 + x * 4) + 2] = colorArray[1];
			canvas[(y * curData.width * 4 + x * 4) + 3] = colorArray[0];
		}
	}
	SDL_UpdateTexture(texture, NULL, canvas.data(), (int)curData.width * 4);
}

void SDL3Helper::LoadPathFileTexture([[maybe_unused]] UltimaVResource* u5_resources)
{
	m_PathFileTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		272, 62);
	unsigned char* pixels = NULL;
	int pitch;
	SDL_LockTexture(m_PathFileTexture, NULL, (void**)&pixels, &pitch);
	memset(pixels, 0x00, sizeof(unsigned char) * pitch * 62);
	SDL_UnlockTexture(m_PathFileTexture);
	SDL_SetTextureScaleMode(m_PathFileTexture, SDL_SCALEMODE_NEAREST);
}

void SDL3Helper::LoadBitFileTextures(UltimaVResource* u5_resources)
{
	m_BitFileTextures.resize(u5_resources->m_BitFileData.size());
	for (int indexBit = 0; indexBit < u5_resources->m_BitFileData.size(); indexBit++)
	{
		m_BitFileTextures[indexBit].resize(u5_resources->m_BitFileData[indexBit].size());
		for (int indexPic = 0; indexPic < u5_resources->m_BitFileData[indexBit].size(); indexPic++)
		{
			U5ImageData& curData = u5_resources->m_BitFileData[indexBit][indexPic];
			CreateTextureFromMemory(m_BitFileTextures[indexBit][indexPic], curData);
		}
	}
	unsigned char transparent_color[3] = { 0, 0, 0 };
	LoadFadeTexture(u5_resources->m_BitFileData[2][0], m_WoDFadeTexture, true, true, transparent_color);
	SDL_SetTextureColorMod(m_WoDFadeTexture, 0, 0, 0);
}

void SDL3Helper::LoadMaskTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha)
{
	texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		data.width, data.height);
	unsigned char* pixels = NULL;
	int pitch;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	memset(pixels, 0xFF, sizeof(unsigned char) * pitch * data.height);
	//if (alpha)
	{
		for (size_t y = 0; y < data.height; y++)
		{
			for (size_t x = 0; x < data.width; x++)
			{
				pixels[(y * pitch) + (x * 4)] = alpha ? 0xFF : 0x00;
			}
		}
	}
	SDL_UnlockTexture(texture);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void SDL3Helper::LoadFadeTexture(U5ImageData& data, SDL_Texture *&texture, bool alpha, bool has_transparent, unsigned char transparent_color[3])
{
	texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		data.width, data.height);
	unsigned char* pixels = NULL;
	int pitch;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
	
	memset(pixels, 0x00, sizeof(unsigned char) * pitch * data.height);
	for (size_t y = 0; y < data.height; y++)
	{
		for (size_t x = 0; x < data.width; x++)
		{
			unsigned char curByte = data.pixel_data[y * data.width + x];
			unsigned char colorArray[3] = {};
			if (data.mode == 8)
			{
				if (curByte == 0)
				{
					std::memcpy(colorArray, ega_table[0], sizeof(colorArray));
				}
				else
				{
					std::memcpy(colorArray, ega_table[15], sizeof(colorArray));
				}
			}
			else if (data.mode == 4)
			{
				if (curByte < 4)
				{
					std::memcpy(colorArray, cga_table[curByte], sizeof(colorArray));
				}
			}
			else
			{
				if (curByte < 16)
				{
					std::memcpy(colorArray, ega_table[curByte], sizeof(colorArray));
				}
			}
			// ABGR
			if (has_transparent)
			{
				if (transparent_color[0] == colorArray[0] &&
					transparent_color[1] == colorArray[1] &&
					transparent_color[2] == colorArray[2])
				{
					pixels[(y * pitch) + (x * 4)] = 0;
				}
				else
				{
					pixels[(y * pitch) + (x * 4)] = alpha ? 0xFF : 0x00;
				}
			}
			else
			{
				pixels[(y * pitch) + (x * 4)] = alpha ? 0xFF : 0x00;
			}
			pixels[(y * pitch) + (x * 4) + 1] = colorArray[2];
			pixels[(y * pitch) + (x * 4) + 2] = colorArray[1];
			pixels[(y * pitch) + (x * 4) + 3] = colorArray[0];
		}
	}
	SDL_UnlockTexture(texture);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void SDL3Helper::TurnOnPixels(SDL_Texture* texture, std::vector<int>& vec_pixels, bool on)
{
	unsigned char* pixels = NULL;
	int pitch;
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);

	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (int cur_pixel : vec_pixels)
	{
		int curY = cur_pixel / static_cast<int>(width);
		int curX = cur_pixel % static_cast<int>(width);

		pixels[(curY * pitch) + (curX * 4)] = on ? 0xFF : 0x00;
	}

	SDL_UnlockTexture(texture);
}

void SDL3Helper::LoadImage16FileTextures(UltimaVResource* u5_resources)
{
	m_Image16FileTextures.resize(u5_resources->m_Image16FileData.size());
	for (int indexBit = 0; indexBit < u5_resources->m_Image16FileData.size(); indexBit++)
	{
		m_Image16FileTextures[indexBit].resize(u5_resources->m_Image16FileData[indexBit].size());
		for (int indexPic = 0; indexPic < u5_resources->m_Image16FileData[indexBit].size(); indexPic++)
		{
			U5ImageData &curData = u5_resources->m_Image16FileData[indexBit][indexPic];
			CreateTextureFromMemory(m_Image16FileTextures[indexBit][indexPic], curData);
		}
	}
	LoadFadeTexture(u5_resources->m_Image16FileData[12][0], m_LogoFadeTexture, false);
	LoadMaskTexture(u5_resources->m_Image16FileData[12][1], m_Flame1FadeTexture, true);
	SDL_SetTextureColorMod(m_Flame1FadeTexture, 0, 0, 0);
}

void SDL3Helper::LoadImageData(UltimaVResource *u5_resources)
{
	LoadBitFileTextures(u5_resources);
	LoadPathFileTexture(u5_resources);
	LoadImage16FileTextures(u5_resources);
}

void SDL3Helper::UpdateTicks()
{
	m_curTick = SDL_GetTicks();
}

Uint64 SDL3Helper::GetCurrentTick() const
{
	return m_curTick;
}