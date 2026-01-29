#include "U5Tile.h"
#include <SDL3/SDL_render.h>
#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include <vector>
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <iterator>

U5Tile::U5Tile() :
	m_texture(nullptr),
	m_render_texture(nullptr),
	m_data(nullptr),
	m_textureType(TextureType::SINGLE),
	m_sdl_helper(nullptr),
	m_secondaryTexture(-1),
	m_animatingMaxValue(0),
	m_animatingCurValue(0),
	m_cur_texture_in_rotation(0),
	m_id(-1)
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

SDL_Texture* U5Tile::GetTexture()
{
	switch (m_textureType)
	{
	case TextureType::ROTATING:
	{
		if (m_cur_texture_in_rotation >= m_textures_in_rotation.size())
		{
			return nullptr;
		}
		size_t curTextureNum = m_textures_in_rotation[m_cur_texture_in_rotation];
		return m_sdl_helper->m_TileTextures[curTextureNum].GetUnderlyingTexture();
	}
	case TextureType::MASKED:
		return m_render_texture;
	case TextureType::SCROLLING:
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
}

void U5Tile::CreateScrollingTexture(Uint64 animation_speed)
{
	m_textureType = TextureType::SCROLLING;
	m_animatingMaxValue = animation_speed;

	m_render_texture = SDL_CreateTexture(m_sdl_helper->m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		static_cast<int>(m_data->width), static_cast<int>(m_data->height));
	SDL_SetTextureScaleMode(m_render_texture, SDL_SCALEMODE_NEAREST);
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
