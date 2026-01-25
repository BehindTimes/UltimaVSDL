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
#include "U5Enums.h"
#include <SDL3/SDL_keycode.h>
#include <cstdint>
#include <string>
#include <SDL3/SDL_blendmode.h>
#include <memory>
#include "U5Input.h"

extern std::unique_ptr<U5Input> m_input;

SDL3Helper::SDL3Helper() :
	m_window(nullptr),
	m_renderer(nullptr),
	m_quit(false),
	m_event(NULL),
	m_curTick(0),
	m_PathFileTexture(nullptr),
	m_LogoFadeTexture(nullptr),
	m_WoDFadeTexture(nullptr),
	m_Flame1FadeTexture(nullptr),
	m_CodexFadeTexture(nullptr),
	m_FullScreenTexture(nullptr),
	m_PromptTexture(nullptr)
{
}

int SDL3Helper::Intialize()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	//if (!SDL_CreateWindowAndRenderer("Ultima V - SDL", 320, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_HIGH_PIXEL_DENSITY*/, &m_window, &m_renderer))
	if (!SDL_CreateWindowAndRenderer("Ultima V - SDL", RENDER_WIDTH, RENDER_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_HIGH_PIXEL_DENSITY*/, &m_window, &m_renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}

	SDL_SetRenderLogicalPresentation(m_renderer, RENDER_WIDTH, RENDER_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	//SDL_SetWindowFullscreen(m_window, true);
	return 0;
}

void SDL3Helper::Cleanup()
{
	for (auto& curTexture : m_TileTextures)
	{
		if (curTexture)
		{
			SDL_DestroyTexture(curTexture);
		}
	}
	m_TileTextures.clear();

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

	for (auto& curCharType : m_CharacterSetsTextures)
	{
		for (auto& curImage : curCharType)
		{
			for (auto& curTexture : curImage)
			{
				if (curTexture)
				{
					SDL_DestroyTexture(curTexture);
				}
			}
		}
	}
	m_CharacterSetsTextures.clear();

	for (auto& curTexture : m_TargetTextures)
	{
		if (curTexture)
		{
			SDL_DestroyTexture(curTexture);
		}
	}
	m_TargetTextures.clear();

	for (auto& curTexture : m_ArrowTextures)
	{
		if (curTexture)
		{
			SDL_DestroyTexture(curTexture);
		}
	}
	m_ArrowTextures.clear();

	for (auto& curTexture : m_ProportionalFontTextures)
	{
		if (curTexture)
		{
			SDL_DestroyTexture(curTexture);
		}
	}
	m_ProportionalFontTextures.clear();

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

	if (m_FullScreenTexture)
	{
		SDL_DestroyTexture(m_FullScreenTexture);
		m_FullScreenTexture = nullptr;
	}

	if (m_CodexFadeTexture)
	{
		SDL_DestroyTexture(m_CodexFadeTexture);
		m_CodexFadeTexture = nullptr;
	}

	if (m_PromptTexture)
	{
		SDL_DestroyTexture(m_PromptTexture);
		m_PromptTexture = nullptr;
	}

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void SDL3Helper::GetScreenDimensions(int &width, int &height)
{
	width = RENDER_WIDTH;
	height = RENDER_HEIGHT;
	//SDL_GetWindowSize(m_window , &width, &height);
}

void SDL3Helper::ClearScreen() const
{
	SDL_RenderClear(m_renderer);
}

void SDL3Helper::DrawInvertRect(int x_tile, int y_tile, int width, int height) const
{
	SDL_BlendMode blendmode_sub = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_SUBTRACT,
		SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_MAXIMUM);

	SDL_FRect toRect{};
	toRect.x = static_cast<float>(x_tile * HALF_TILE_HEIGHT);
	toRect.y = static_cast<float>(y_tile * HALF_TILE_HEIGHT);
	toRect.w = static_cast<float>(width * HALF_TILE_HEIGHT);
	toRect.h = static_cast<float>(height * HALF_TILE_HEIGHT);

	//SDL_SetRenderDrawBlendMode(m_renderer, blendmode_sub);
	//SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawBlendMode(m_renderer, blendmode_sub);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(m_renderer, &toRect);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
	
}

void SDL3Helper::DrawTiledText(std::string text, int x_tile, int y_tile)
{
	SDL_FRect toRect{};
	toRect.x = 0;
	toRect.y = static_cast<float>(y_tile * HALF_TILE_HEIGHT);
	toRect.w = HALF_TILE_WIDTH;
	toRect.h = HALF_TILE_HEIGHT;
	SDL_Texture* texture = nullptr;

	for (size_t index = 0; index < text.size(); index++)
	{
		if (text[index] < 128)
		{
			texture = m_CharacterSetsTextures[0][0][text[index]];
			toRect.x = (x_tile * HALF_TILE_WIDTH) + static_cast<float>(index * HALF_TILE_WIDTH);
			SDL_RenderTexture(m_renderer, texture, NULL, &toRect);
		}
	}
}

void SDL3Helper::DrawTileRect(int x_tile, int y_tile) const
{
	SDL_FRect toRect{};
	toRect.x = 0;
	toRect.y = static_cast<float>(y_tile * HALF_TILE_HEIGHT);
	toRect.x = static_cast<float>(x_tile * HALF_TILE_WIDTH);
	toRect.w = HALF_TILE_WIDTH;
	toRect.h = HALF_TILE_HEIGHT;
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xFF);
	SDL_RenderFillRect(m_renderer, &toRect);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
}

void SDL3Helper::DrawTileTexture8(SDL_Texture* texture, int x_tile, int y_tile) const
{
	SDL_FRect toRect{};
	toRect.x = 0;
	toRect.y = static_cast<float>(y_tile * HALF_TILE_HEIGHT);
	toRect.x = static_cast<float>(x_tile * HALF_TILE_WIDTH);
	toRect.w = HALF_TILE_WIDTH;
	toRect.h = HALF_TILE_HEIGHT;

	SDL_RenderTexture(m_renderer, texture, NULL, &toRect);
}

void SDL3Helper::DrawTileTexture(SDL_Texture* texture, int x_tile, int y_tile) const
{
	SDL_FRect toRect{};
	toRect.x = 0;
	toRect.y = static_cast<float>(y_tile * RENDER_TILE_HEIGHT);
	toRect.x = static_cast<float>(x_tile * RENDER_TILE_WIDTH);
	toRect.w = RENDER_TILE_WIDTH;
	toRect.h = RENDER_TILE_HEIGHT;

	SDL_RenderTexture(m_renderer, texture, NULL, &toRect);
}

void SDL3Helper::RenderTextureFromTo(SDL_Texture* texture, float from_x, float from_y, float from_width, float from_height,
	float to_x, float to_y, float to_width, float to_height) const
{
	SDL_FRect fromRect{};
	fromRect.x = static_cast<float>(from_x);
	fromRect.y = static_cast<float>(from_y);
	fromRect.w = static_cast<float>(from_width);
	fromRect.h = static_cast<float>(from_height);

	SDL_FRect toRect{};
	toRect.x = static_cast<float>(to_x);
	toRect.y = static_cast<float>(to_y);
	toRect.w = static_cast<float>(to_width);
	toRect.h = static_cast<float>(to_height);

	if (texture)
	{
		SDL_RenderTexture(m_renderer, texture, &fromRect, &toRect);
	}
}

void SDL3Helper::RenderTextureAt(SDL_Texture* texture, float x, float y, float width, float height) const
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

void SDL3Helper::RenderFlipTextureAt(SDL_Texture* texture, float x, float y, float width, float height, bool flip) const
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

void SDL3Helper::RenderPresent() const
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
	m_input->StartInput();
	SDL_PollEvent(&m_event);
	switch (m_event.type)
	{
	case SDL_EVENT_QUIT:
		m_quit = true;
		break;
	case SDL_EVENT_KEY_DOWN:
		if (m_event.key.mod & SDL_KMOD_ALT)
		{
			if (m_event.key.key == SDLK_X)
			{
				m_quit = true;
			}
		}
		m_input->ProcessKeyDown(m_event.key);
		break;
	}
	m_input->FinishInput();
}

void SDL3Helper::CreateTextureFromMemory(SDL_Texture *&texture, const U5ImageData &curData, bool has_transparent, unsigned char transparent_color[3]) const
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

			unsigned char alpha = 0xFF;
			if (has_transparent)
			{
				if (transparent_color[0] == colorArray[0] &&
					transparent_color[1] == colorArray[1] &&
					transparent_color[2] == colorArray[2])
				{
					alpha = 0;
				}
			}
			
			// ABGR
			canvas[(y * curData.width * 4 + x * 4) + 0] = alpha;
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

// For real outlines, we should check that the entire border is black, rather than just the pixel next to it,
// but this is used for characters, where we won't have to be as specific
void SDL3Helper::CreateOutlineTexture(SDL_Texture*& texture, const U5ImageData& curData, unsigned char color_data[3]) const
{
	texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
		static_cast<int>(curData.width), static_cast<int>(curData.height));
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	std::vector<unsigned char> canvas;
	size_t canvasSize = static_cast<size_t>(curData.width) * static_cast<size_t>(curData.height) * 4;
	canvas.resize(canvasSize);
	std::fill(canvas.begin(), canvas.end(), static_cast<unsigned char>(0));

	unsigned char color_white[3] = { 0xFF, 0xFF, 0xFF };

	for (size_t y = 0; y < curData.height; y++)
	{
		for (size_t x = 0; x < curData.width; x++)
		{
			unsigned char curByte = curData.pixel_data[y * curData.width + x];

			if (curByte != 0)
			{
				bool outline = false;
				if (x > 0)
				{
					if (curData.pixel_data[y * curData.width + (x - 1)] == 0)
					{
						outline = true;
					}
				}
				if (x < static_cast<size_t>(curData.width) - 1)
				{
					if (curData.pixel_data[y * curData.width + (x + 1)] == 0)
					{
						outline = true;
					}
				}
				if (y > 0)
				{
					if (curData.pixel_data[(y - 1) * curData.width + x] == 0)
					{
						outline = true;
					}
				}
				if (y < static_cast<size_t>(curData.height - 1))
				{
					if (curData.pixel_data[(y + 1) * curData.width + x] == 0)
					{
						outline = true;
					}
				}
				if (outline)
				{
					canvas[(y * curData.width * 4 + x * 4) + 0] = 0xFF;
					canvas[(y * curData.width * 4 + x * 4) + 1] = color_white[2];
					canvas[(y * curData.width * 4 + x * 4) + 2] = color_white[1];
					canvas[(y * curData.width * 4 + x * 4) + 3] = color_white[0];
				}
				else
				{
					canvas[(y * curData.width * 4 + x * 4) + 0] = 0xFF;
					canvas[(y * curData.width * 4 + x * 4) + 1] = color_data[2];
					canvas[(y * curData.width * 4 + x * 4) + 2] = color_data[1];
					canvas[(y * curData.width * 4 + x * 4) + 3] = color_data[0];
				}
			}
		}
	}
	SDL_UpdateTexture(texture, NULL, canvas.data(), (int)curData.width * 4);
}

void SDL3Helper::LoadCharacterSetTextures(UltimaVResource* u5_resources)
{
	m_CharacterSetsTextures.resize(u5_resources->m_CharacterSetsData.size());
	for (int indexCharType = 0; indexCharType < u5_resources->m_CharacterSetsData.size(); indexCharType++)
	{
		m_CharacterSetsTextures[indexCharType].resize(u5_resources->m_CharacterSetsData[indexCharType].size());
		for (int indexPic = 0; indexPic < u5_resources->m_CharacterSetsData[indexCharType].size(); indexPic++)
		{
			m_CharacterSetsTextures[indexCharType][indexPic].resize(u5_resources->m_CharacterSetsData[indexCharType][indexPic].size());
			for (int indexChar = 0; indexChar < u5_resources->m_CharacterSetsData[indexCharType][indexPic].size(); indexChar++)
			{
				U5ImageData& curData = u5_resources->m_CharacterSetsData[indexCharType][indexPic][indexChar];
				CreateTextureFromMemory(m_CharacterSetsTextures[indexCharType][indexPic][indexChar], curData);
			}
		}
	}
	m_ArrowTextures.resize(2);
	m_ArrowTextures.resize(2);
	if (u5_resources->m_render_mode == RenderMode::CGA)
	{
		CreateOutlineTexture(m_ArrowTextures[0], u5_resources->m_CharacterSetsData[0][0][1], cga_table[1]);
		CreateOutlineTexture(m_ArrowTextures[1], u5_resources->m_CharacterSetsData[0][0][2], cga_table[1]);
	}
	else
	{
		CreateOutlineTexture(m_ArrowTextures[0], u5_resources->m_CharacterSetsData[0][0][1], ega_table[1]);
		CreateOutlineTexture(m_ArrowTextures[1], u5_resources->m_CharacterSetsData[0][0][2], ega_table[1]);
	}
	
}

void SDL3Helper::LoadMaskTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha) const
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

void SDL3Helper::LoadFadeTexture(U5ImageData& data, SDL_Texture *&texture, bool alpha, bool has_transparent, unsigned char transparent_color[3]) const
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

void SDL3Helper::ClearStreamingTexture(SDL_Texture* texture)
{
	unsigned char* pixels = NULL;
	int pitch;
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);

	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	memset(pixels, 0, sizeof(unsigned char) * static_cast<size_t>(width * height * 4));

	SDL_UnlockTexture(texture);
}

void SDL3Helper::TurnOnAllPixels(SDL_Texture* texture, bool on)
{
	unsigned char* pixels = NULL;
	int pitch;
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);

	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (int indexY = 0; indexY < height; indexY++)
	{
		for (int indexX = 0; indexX < pitch; indexX += 4)
		{
			pixels[(indexY * pitch) + indexX] = on ? 0xFF : 0x00;
		}
	}

	SDL_UnlockTexture(texture);
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
			unsigned char trans_color[3] = { 0,0,0 };
			CreateTextureFromMemory(m_Image16FileTextures[indexBit][indexPic], curData, IV16_TEXT == indexBit, trans_color);
		}
	}
	LoadFadeTexture(u5_resources->m_Image16FileData[12][0], m_LogoFadeTexture, false);
	LoadMaskTexture(u5_resources->m_Image16FileData[12][1], m_Flame1FadeTexture, true);
	SDL_SetTextureColorMod(m_Flame1FadeTexture, 0, 0, 0);
}

void SDL3Helper::LoadTileTextures(UltimaVResource* u5_resources)
{
	m_TileTextures.resize(u5_resources->m_Tiles.size());
	for (int indexPic = 0; indexPic < u5_resources->m_Tiles.size(); indexPic++)
	{
		U5ImageData& curData = u5_resources->m_Tiles[indexPic];
		CreateTextureFromMemory(m_TileTextures[indexPic], curData);
	}
}

void SDL3Helper::LoadProportionalFontTextures(UltimaVResource* u5_resources)
{
	m_ProportionalFontTextures.resize(u5_resources->m_ProportionalFontData.size());
	unsigned char black[3] = { 0,0,0 };
	for (size_t index = 0; index < u5_resources->m_ProportionalFontData.size(); index++)
	{
		CreateTextureFromMemory(m_ProportionalFontTextures[index], u5_resources->m_ProportionalFontData[index], true, black);
	}
}

void SDL3Helper::LoadStreamingTextures()
{
	m_CodexFadeTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		36, 34);
	SDL_SetTextureScaleMode(m_CodexFadeTexture, SDL_SCALEMODE_NEAREST);
}

void SDL3Helper::LoadTargetTextures()
{
	m_TargetTextures.resize(2);
	m_TargetTextures[TTV_INTROBOX] = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		20 * RENDER_TILE_WIDTH, 5 * RENDER_TILE_HEIGHT);
	SDL_SetTextureScaleMode(m_TargetTextures[TTV_INTROBOX], SDL_SCALEMODE_NEAREST);
	m_TargetTextures[TTV_INTROBOX_DISPLAY] = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		19 * RENDER_TILE_WIDTH, 4 * RENDER_TILE_HEIGHT);
	SDL_SetTextureScaleMode(m_TargetTextures[TTV_INTROBOX_DISPLAY], SDL_SCALEMODE_NEAREST);

	m_FullScreenTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		RENDER_WIDTH, RENDER_HEIGHT);

	m_PromptTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
}

void SDL3Helper::LoadImageData(UltimaVResource *u5_resources)
{
	LoadBitFileTextures(u5_resources);
	LoadPathFileTexture(u5_resources);
	LoadTileTextures(u5_resources);
	LoadImage16FileTextures(u5_resources);
	LoadCharacterSetTextures(u5_resources);
	LoadProportionalFontTextures(u5_resources);
	LoadTargetTextures();
	LoadStreamingTextures();
}

void SDL3Helper::UpdateTicks()
{
	m_curTick = SDL_GetTicks();
}

Uint64 SDL3Helper::GetCurrentTick() const
{
	return m_curTick;
}

void SDL3Helper::SetRenderTarget(SDL_Texture* texture) const
{
	SDL_SetRenderTarget(m_renderer, texture);
}

void SDL3Helper::CopyTextureToStreaming(U5ImageData &texture, SDL_Texture *streaming_texture, uint32_t width, uint32_t height)
{
	float tex_width, tex_height;
	SDL_GetTextureSize(streaming_texture, &tex_width, &tex_height);

	// Not going to be 100% compliant here.
	if (width < tex_width || height < tex_height ||
		texture.width < width|| texture.height < height)
	{
		ClearStreamingTexture(streaming_texture);
		return;
	}

	unsigned char* pixels = NULL;
	int pitch;
	ClearStreamingTexture(streaming_texture);
	SDL_LockTexture(streaming_texture, NULL, (void**)&pixels, &pitch);
	unsigned char color_data[3];

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			if (x > tex_width)
			{
				break;
			}
			unsigned char curPixel = texture.pixel_data[texture.width * y + x];

			if (texture.mode == 4)
			{
				std::memcpy(color_data, cga_table[curPixel], sizeof(color_data));
			}
			else
			{
				std::memcpy(color_data, ega_table[curPixel], sizeof(color_data));
			}

			//pixels[(y * pitch) + (x * 4)] = alpha ? 0xFF : 0x00;
			pixels[(y * pitch + (x * 4)) + 0] = 0x00;
			pixels[(y * pitch + (x * 4)) + 1] = color_data[2];
			pixels[(y * pitch + (x * 4)) + 2] = color_data[1];
			pixels[(y * pitch + (x * 4)) + 3] = color_data[0];
		}
		if (y > tex_height)
		{
			break;
		}
	}

	SDL_UnlockTexture(streaming_texture);
}
