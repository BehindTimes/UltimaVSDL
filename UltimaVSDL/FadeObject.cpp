#include "FadeObject.h"
#include <SDL3/SDL_stdinc.h>
#include "U5Utils.h"
#include <vector>
#include <cstdint>
#include <memory>
#include <SDL3/SDL_render.h>
#include "SDL3Helper.h"
#include "UltimaVResource.h"

extern std::unique_ptr<U5Utils> m_utilities;

FadeObject::FadeObject(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	m_delay(0),
	m_curFade(0),
	m_isFading(false),
	m_num_pixels(0),
	m_fade_count(0),
	m_cur_count(0),
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources)
{
}

void FadeObject::Reset()
{
	m_curFade = 0;
	m_cur_count = m_fade_count;
	m_locations.clear();
	m_locations.resize(m_fade_count);
	m_num_pixels = 0;
}

void FadeObject::SetDelay(Uint64 delay)
{
	m_delay = delay;
}

bool FadeObject::IsFading() const
{
	return m_curFade < m_delay;
}

void FadeObject::SetSize(int count)
{
	m_fade_count = count;
	m_cur_count = m_fade_count;
	m_locations.resize(m_fade_count);
}

void FadeObject::AddElapsedTime(Uint64 elapsed_time, SDL_Texture* curTexture, bool turn_on)
{
	if (m_curFade < m_delay)
	{
		m_curFade += elapsed_time;
		// Turn on all the pixels now
		if (m_curFade >= m_delay && curTexture != nullptr)
		{
			m_sdl_helper->TurnOnAllPixels(curTexture, turn_on);
		}
	}
}

bool FadeObject::ProcessFade(SDL_Texture* curTexture, bool turn_on)
{
	uint32_t num_pixels = 0;
	uint32_t total_pixels = static_cast<uint32_t>(((m_curFade) / static_cast<float>(m_delay)) * m_fade_count);

	if (total_pixels >= m_num_pixels)
	{
		num_pixels = total_pixels - m_num_pixels;
		m_num_pixels = total_pixels;
	}

	if (m_cur_count == 0)
	{
		m_num_pixels = 0;
		return true;
	}

	// Set the on pixels now
	for (uint32_t index = 0; index < num_pixels; index++)
	{
		uint32_t curOnPixel = m_utilities->GetRandom(0, m_cur_count);
		if (m_cur_count > 0)
		{
			m_cur_count--;
		}
		else
		{
			break;
		}
		uint32_t cur_vector_index = 0;
		std::vector<int> vec_on;

		for (size_t vector_index = 0; vector_index < m_locations.size(); vector_index++)
		{
			if (m_locations[vector_index] != 0)
			{
				continue;
			}
			if (cur_vector_index == curOnPixel)
			{
				m_locations[vector_index] = 1;
				vec_on.push_back(static_cast<int>(vector_index));
				break;
			}
			cur_vector_index++;
		}

		if (vec_on.size() > 0)
		{
			m_sdl_helper->TurnOnPixels(curTexture, vec_on, turn_on);
		}
	}
	return false;
}
