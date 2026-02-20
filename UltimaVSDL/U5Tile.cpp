#include "U5Tile.h"
#include <SDL3/SDL_render.h>
#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include <vector>
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include "U5Utils.h"
#include "ColorData.h"
#include <cstring>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_rect.h>

extern std::unique_ptr<U5Utils> m_utilities;

U5Tile::U5Tile() :
	m_texture(nullptr),
	m_render_texture(nullptr),
	m_data(nullptr),
	m_data_mask(nullptr),
	m_textureType(TextureType::SINGLE),
	m_sdl_helper(nullptr),
	m_secondaryTexture(-1),
	m_animatingMaxValue(0),
	m_animatingCurValue(0),
	m_cur_texture_in_rotation(0),
	m_id(-1),
	m_render_mode(RenderMode::EGA)
{
}

U5Tile::~U5Tile()
{
	if (m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
	if (m_render_texture)
	{
		SDL_DestroyTexture(m_render_texture);
		m_render_texture = nullptr;
	}
}
SDL_Texture* U5Tile::GetUnderlyingTexture()
{
	return m_texture;
}

TextureType U5Tile::GetTextureType() const
{
	return m_textureType;
}

SDL_Texture* U5Tile::GetTexture()
{
	switch (m_textureType)
	{
	case TextureType::NPC:
	case TextureType::ROTATING:
	{
		if (m_cur_texture_in_rotation >= m_textures_in_rotation.size())
		{
			return nullptr;
		}
		size_t curTextureNum = m_textures_in_rotation[m_cur_texture_in_rotation];
		return m_sdl_helper->m_TileTextures[curTextureNum].GetUnderlyingTexture();
	}
	case TextureType::FLAG:
		if (m_cur_texture_in_rotation == 0)
		{
			return m_texture;
		}
		else
		{
			return m_render_texture;
		}
		break;
	case TextureType::MASKED:
		return m_render_texture;
	case TextureType::SCROLLING:
		return m_render_texture;
	case TextureType::FIRE:
		return m_render_texture;
	default:
		return m_texture;
	}
}

void U5Tile::CreateTexture(int id, SDL3Helper* sdlHelper, U5ImageData* data)
{
	m_id = id;
	sdlHelper->CreateTextureFromMemory(m_texture, *data);
	m_data = data;
	m_sdl_helper = sdlHelper;
}

void U5Tile::CreateMaskTexture(int id, SDL3Helper* sdlHelper, U5ImageData* data, U5ImageData* mask, int underlying_texture)
{
	m_textureType = TextureType::MASKED;
	m_id = id;
	sdlHelper->CreateTextureFromMemoryWithMask(m_texture, m_render_texture , *data, *mask);
	m_data = data;
	m_sdl_helper = sdlHelper;
	m_secondaryTexture = underlying_texture;
	UpdateTime(0);
}

void U5Tile::CreateRotationTextures(std::vector<int> textures, Uint64 animation_speed)
{
	m_textureType = TextureType::ROTATING;
	m_textures_in_rotation = textures;
	m_animatingMaxValue = animation_speed;
	m_cur_texture_in_rotation = m_id;

	std::vector<int>::iterator it = std::find(m_textures_in_rotation.begin(), m_textures_in_rotation.end(), m_id);
	if (it == m_textures_in_rotation.end())
	{
		m_cur_texture_in_rotation = 0;
	}
	m_cur_texture_in_rotation = std::distance(m_textures_in_rotation.begin(), it);
	UpdateTime(0);
}

void U5Tile::CreateNPCRotationTextures(Uint64 animation_speed)
{
	m_textureType = TextureType::NPC;
	int startPos = m_id - (m_id % 4);
	for (int index = 0; index < 4; index++)
	{
		m_textures_in_rotation.emplace_back(startPos + index);
	}
	m_animatingMaxValue = animation_speed;
	m_cur_texture_in_rotation = m_id;

	std::vector<int>::iterator it = std::find(m_textures_in_rotation.begin(), m_textures_in_rotation.end(), m_id);
	if (it == m_textures_in_rotation.end())
	{
		m_cur_texture_in_rotation = 0;
	}
	m_cur_texture_in_rotation = std::distance(m_textures_in_rotation.begin(), it);
	UpdateTime(0);
}

void U5Tile::CreateScrollingTexture(Uint64 animation_speed)
{
	m_textureType = TextureType::SCROLLING;
	m_animatingMaxValue = animation_speed;

	m_render_texture = SDL_CreateTexture(m_sdl_helper->m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		static_cast<int>(m_data->width), static_cast<int>(m_data->height));
	SDL_SetTextureScaleMode(m_render_texture, SDL_SCALEMODE_NEAREST);
	UpdateTime(0);
}

void U5Tile::CreateFlagTexture(Uint64 animation_speed, int xpos, int ypos, int width, int height)
{
	m_textureType = TextureType::FLAG;
	m_animatingMaxValue = animation_speed;

	m_render_texture = SDL_CreateTexture(m_sdl_helper->m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		static_cast<int>(m_data->width), static_cast<int>(m_data->height));
	SDL_SetTextureScaleMode(m_render_texture, SDL_SCALEMODE_NEAREST);

	SDL_FRect toRect{};
	SDL_FRect fromRect{};
	toRect.x = static_cast<float>(xpos);
	toRect.y = static_cast<float>(ypos);
	toRect.w = static_cast<float>(width);
	toRect.h = static_cast<float>(height);
	fromRect.x = static_cast<float>(xpos);
	fromRect.y = static_cast<float>(ypos);
	fromRect.w = static_cast<float>(width);
	fromRect.h = static_cast<float>(height);

	m_sdl_helper->SetRenderTarget(m_render_texture);
	SDL_RenderTexture(m_sdl_helper->m_renderer, m_texture, NULL, NULL);

	SDL_RenderTextureRotated(m_sdl_helper->m_renderer, m_texture, &fromRect, &toRect, 0, NULL, SDL_FLIP_VERTICAL);

	//SDL_RenderTextureRotated(m_sdl_helper->m_renderer, m_texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);

	m_sdl_helper->SetRenderTarget(nullptr);

	UpdateTime(0);
}

void U5Tile::CreateFireTexture(U5ImageData* mask, Uint64 animation_speed, RenderMode render_mode)
{
	m_textureType = TextureType::FIRE;
	m_animatingMaxValue = animation_speed;
	m_data_mask = mask;
	m_render_mode = render_mode;

	m_render_texture = SDL_CreateTexture(m_sdl_helper->m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		static_cast<int>(m_data->width), static_cast<int>(m_data->height));
	SDL_SetTextureScaleMode(m_render_texture, SDL_SCALEMODE_NEAREST);
	m_sdl_helper->CopyTextureToStreaming(*m_data, m_render_texture, m_data->width, m_data->height, true);
	UpdateTime(0);
}

void U5Tile::UpdateFireTexture()
{
	unsigned char* pixels = NULL;
	int pitch;
	unsigned char colorArray[3] = {};
	unsigned char blackArray[3] = { 0, 0, 0 };
	unsigned char color1Array[3] = {};
	unsigned char color2Array[3] = {};
	unsigned char magentaArray[3] = { 0xAA,0x00,0xAA };

	SDL_LockTexture(m_render_texture, NULL, (void**)&pixels, &pitch);

	size_t curPos = 0;
	for (size_t indexY = 0; indexY < m_data->height; indexY++)
	{
		for (size_t indexX = 0; indexX < m_data->width; indexX++)
		{
			if (m_data_mask->pixel_data[curPos] != 0)
			{
				int val = m_utilities->GetRandom(0, 1);
				if (m_render_mode == RenderMode::EGA)
				{
					// If the colors are the same, alternate between on and off.
					// If the main texture is black, use the mask texture
					// color, otherwise alternate between bright and dark red
					if (m_data_mask->pixel_data[curPos] == m_data->pixel_data[curPos])
					{
						std::memcpy(colorArray, ega_table[m_data->pixel_data[curPos]], sizeof(colorArray));

						// ABGR
						pixels[((indexY * pitch) + (indexX * 4)) + 1] = val ? colorArray[2] : blackArray[2];
						pixels[((indexY * pitch) + (indexX * 4)) + 2] = val ? colorArray[1] : blackArray[1];
						pixels[((indexY * pitch) + (indexX * 4)) + 3] = val ? colorArray[0] : blackArray[0];
					}
					else if (m_data->pixel_data[curPos] == 0)
					{
						std::memcpy(colorArray, ega_table[m_data_mask->pixel_data[curPos]], sizeof(colorArray));
						pixels[((indexY * pitch) + (indexX * 4)) + 1] = val ? colorArray[2] : blackArray[2];
						pixels[((indexY * pitch) + (indexX * 4)) + 2] = val ? colorArray[1] : blackArray[1];
						pixels[((indexY * pitch) + (indexX * 4)) + 3] = val ? colorArray[0] : blackArray[0];
					}
					else
					{
						std::memcpy(color1Array, ega_table[m_data->pixel_data[curPos]], sizeof(color1Array));
						if (m_data->pixel_data[curPos] < 8)
						{
							std::memcpy(color2Array, ega_table[m_data->pixel_data[curPos] + 8], sizeof(color2Array));
						}
						else
						{
							std::memcpy(color2Array, ega_table[m_data->pixel_data[curPos] - 8], sizeof(color2Array));
						}

						pixels[((indexY * pitch) + (indexX * 4)) + 1] = val ? color1Array[2] : color2Array[2];
						pixels[((indexY * pitch) + (indexX * 4)) + 2] = val ? color1Array[1] : color2Array[1];
						pixels[((indexY * pitch) + (indexX * 4)) + 3] = val ? color1Array[0] : color2Array[0];
					}
				}
				else if (m_render_mode == RenderMode::CGA)
				{
					// Alternate between on and off.  The mask actually has multiple
					// colors, but unlike EGA, it doesn't change.  I wonder if this
					// is a mistake.
					if (m_data->pixel_data[curPos] == 2 || m_data_mask->pixel_data[curPos] == 3)
					{
						// ABGR
						pixels[((indexY * pitch) + (indexX * 4)) + 1] = val ? magentaArray[2] : blackArray[2];
						pixels[((indexY * pitch) + (indexX * 4)) + 2] = val ? magentaArray[1] : blackArray[1];
						pixels[((indexY * pitch) + (indexX * 4)) + 3] = val ? magentaArray[0] : blackArray[0];
					}
				}
			}
			curPos++;
		}
	}

	SDL_UnlockTexture(m_render_texture);
}

void U5Tile::UpdateTime(Uint64 elapsedTime)
{
	switch (m_textureType)
	{
	case TextureType::ROTATING:
		m_animatingCurValue += elapsedTime;
		if (m_animatingCurValue >= m_animatingMaxValue)
		{
			m_animatingCurValue %= m_animatingMaxValue;
			m_cur_texture_in_rotation++;
			if (m_cur_texture_in_rotation >= m_textures_in_rotation.size())
			{
				m_cur_texture_in_rotation = 0;
			}
		}
		break;
	case TextureType::FLAG:
		m_animatingCurValue += elapsedTime;
		if (m_animatingCurValue >= m_animatingMaxValue)
		{
			m_animatingCurValue %= m_animatingMaxValue;
			m_cur_texture_in_rotation = m_utilities->GetRandom(0, 2);
		}
		break;
	case TextureType::NPC:
		m_animatingCurValue += elapsedTime;
		if (m_animatingCurValue >= m_animatingMaxValue)
		{
			m_animatingCurValue %= m_animatingMaxValue;
			if (m_textures_in_rotation.size() == 0)
			{
				return;
			}
			int val = m_utilities->GetRandom(0, 2);
			if (val == 0)
			{
				if (m_cur_texture_in_rotation == 0)
				{
					m_cur_texture_in_rotation = static_cast<size_t>(m_textures_in_rotation.size() - 1);
				}
				else
				{
					m_cur_texture_in_rotation--;
				}
			}
			else if (val == 1)
			{
				m_cur_texture_in_rotation++;
				if (m_cur_texture_in_rotation >= m_textures_in_rotation.size())
				{
					m_cur_texture_in_rotation = 0;
				}
			}
		}
		break;
	case TextureType::FIRE:
		m_animatingCurValue += elapsedTime;
		if (m_animatingCurValue >= m_animatingMaxValue)
		{
			m_animatingCurValue %= m_animatingMaxValue;
			UpdateFireTexture();
		}
		break;
	case TextureType::MASKED:
		m_sdl_helper->SetRenderTarget(m_render_texture);
		SDL_RenderTexture(m_sdl_helper->m_renderer, m_sdl_helper->m_TileTextures[m_secondaryTexture].GetTexture(), NULL, NULL);
		SDL_RenderTexture(m_sdl_helper->m_renderer, m_texture, NULL, NULL);
		m_sdl_helper->SetRenderTarget(nullptr);
		break;
	case TextureType::SCROLLING:
	{
		m_animatingCurValue += elapsedTime;
		if (m_animatingCurValue >= m_animatingMaxValue)
		{
			m_animatingCurValue %= m_animatingMaxValue;
		}

		float ratio = static_cast<float>(m_animatingCurValue) / m_animatingMaxValue;

		m_sdl_helper->SetRenderTarget(m_render_texture);
		float ystart = m_data->height * ratio;
		m_sdl_helper->RenderTextureFromTo(m_texture, 0, 0, static_cast<float>(m_data->width), static_cast<float>(m_data->height),
			0, ystart, static_cast<float>(m_data->width), static_cast<float>(m_data->height));
		m_sdl_helper->RenderTextureFromTo(m_texture, 0, 0, static_cast<float>(m_data->width), static_cast<float>(m_data->height),
			0, ystart - m_data->height, static_cast<float>(m_data->width), static_cast<float>(m_data->height));
		//SDL_RenderTexture(m_sdl_helper->m_renderer, m_sdl_helper->m_TileTextures[m_secondaryTexture].GetTexture(), NULL, NULL);
		//SDL_RenderTexture(m_sdl_helper->m_renderer, m_texture, NULL, NULL);
		m_sdl_helper->SetRenderTarget(nullptr);
	}
		break;
	default:
		break;
	}
}
