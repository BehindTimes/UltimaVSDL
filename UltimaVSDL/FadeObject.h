#pragma once

#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <vector>
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <SDL3/SDL_render.h>

class FadeObject
{
public:
	explicit FadeObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources);

	void SetSize(int count);
	void SetDelay(Uint64 delay);
	bool IsFading() const;
	void AddElapsedTime(Uint64 elapsed_time, SDL_Texture* curTexture = nullptr, bool turn_on = true);
	bool ProcessFade(SDL_Texture* curTexture, bool turn_on = true);
	void Reset();
private:
	Uint64 m_delay;
	Uint64 m_curFade;
	bool m_isFading;
	uint32_t m_num_pixels;
	int m_fade_count;
	int m_cur_count;
	std::vector<uint8_t> m_locations;
	SDL3Helper* m_sdl_helper;
	UltimaVResource* m_resources;
};

