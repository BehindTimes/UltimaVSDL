#pragma once
#include <SDL3/SDL_render.h>
#include "UltimaVResource.h"
#include <vector>
#include <SDL3/SDL_stdinc.h>

class SDL3Helper;

enum class TextureType
{
	SINGLE,
	SCROLLING,
	MASKED,
	ROTATING,
	FIRE
};

class U5Tile
{
public:
	explicit U5Tile();
	~U5Tile();
	SDL_Texture* GetTexture();
	SDL_Texture* GetUnderlyingTexture();
	void CreateTexture(int id, SDL3Helper* sdlHelper, U5ImageData* data);
	void CreateMaskTexture(int id, SDL3Helper* sdlHelper, U5ImageData* data, U5ImageData* mask, int underlying_texture);
	void CreateFireTexture(U5ImageData* mask, Uint64 animation_speed, RenderMode render_mode);
	void CreateRotationTextures(std::vector<int> textures, Uint64 animation_speed);
	void CreateScrollingTexture(Uint64 animation_speed);
	void UpdateTime(Uint64 elapsedTime);
private:
	void UpdateFireTexture();

	SDL_Texture* m_texture;
	SDL_Texture* m_render_texture;
	U5ImageData* m_data;
	U5ImageData* m_data_mask;
	SDL3Helper* m_sdl_helper;
	int m_secondaryTexture;
	TextureType m_textureType;
	Uint64 m_animatingMaxValue;
	Uint64 m_animatingCurValue;
	std::vector<int> m_textures_in_rotation;
	size_t m_cur_texture_in_rotation;
	int m_id;
	RenderMode m_render_mode;
};

