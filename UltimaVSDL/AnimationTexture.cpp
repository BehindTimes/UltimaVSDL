#include "AnimationTexture.h"

AnimationTexture::AnimationTexture(SDL_Texture* original_texture, SDL_Texture* target_texture, Uint64 total_time) :
	m_original_texture(original_texture),
	m_target_texture(target_texture),
	m_total_time(total_time),
	m_current_time(0)
{
	SDL_GetTextureSize(original_texture, &m_original_width, &m_original_height);
	SDL_GetTextureSize(target_texture, &m_width, &m_height);
}

void AnimationTexture::SetCurrentTime(Uint64 current_time)
{
}

void AnimationTexture::UpdateTime(Uint64 elapsed_time)
{
}

