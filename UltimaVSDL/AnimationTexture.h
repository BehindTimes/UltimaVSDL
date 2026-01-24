#pragma once
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

class AnimationTexture
{
public:
	explicit AnimationTexture(SDL_Texture* original_texture, SDL_Texture* target_texture, Uint64 total_time);
	void SetCurrentTime(Uint64 current_time);
	void UpdateTime(Uint64 elapsed_time);
private:
	SDL_Texture* m_target_texture;
	SDL_Texture* m_original_texture;
	Uint64 m_current_time;
	Uint64 m_total_time;
	float m_width;
	float m_height;
	float m_original_width;
	float m_original_height;
};

