#include "Intro.h"
#include "U5Utils.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"

extern std::unique_ptr<U5Utils> m_utilities;

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0),
	m_curMode(IntroMode::FADE_LOGO),
	m_curLogoFade(0),
	m_window_width(0),
	m_window_height(0)
{
	m_clearScreen = true;

	m_logo_fade_count = static_cast<int>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width);
	m_cur_logo_count = m_logo_fade_count;
	m_logo_fade_locations.resize(static_cast<size_t>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width));
}

Intro::~Intro()
{
}

void Intro::RenderFadeIn()
{
	float width;
	float height;

	SDL_Texture* curTexture;
	Uint64 old_fade = m_curLogoFade;
	if (m_curLogoFade == 0)
	{
		m_curLogoFade = m_tickElapse;
		return;
	}
	m_curLogoFade += m_tickElapse;
	uint32_t num_pixels = 0;

	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	if (m_curLogoFade > LOGO_FADE_DELAY)
	{
		m_curMode = IntroMode::SHOW_ALL;
		RenderLogo();
		return;
	}
	else
	{
		num_pixels = static_cast<uint32_t>(((m_curLogoFade - old_fade) / static_cast<float>(LOGO_FADE_DELAY)) * m_logo_fade_count);
		if (m_cur_logo_count == 0)
		{
			num_pixels = 0;
			RenderLogo();
			return;
		}
	}
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_LogoFadeTexture;
	std::vector<int> vec_on;

	// Set the on pixels now
	for (uint32_t index = 0; index < num_pixels; index++)
	{
		uint32_t curOnPixel = m_utilities->GetRandom(0, m_cur_logo_count);
		if (m_cur_logo_count > 0)
		{
			m_cur_logo_count--;
		}
		else
		{
			break;
		}
		uint32_t cur_vector_index = 0;
		
		for (size_t vector_index = 0; vector_index < m_logo_fade_locations.size(); vector_index++)
		{
			if (m_logo_fade_locations[vector_index] != 0)
			{
				continue;
			}
			if (cur_vector_index == curOnPixel)
			{
				m_logo_fade_locations[vector_index] = 1;
				vec_on.push_back(static_cast<int>(vector_index));
				break;
			}
			cur_vector_index++;
		}
	}
	if (vec_on.size() > 0)
	{
		m_sdl_helper->TurnOnPixels(curTexture, vec_on);
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 0;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderLogo()
{
	float width;
	float height;
	
	SDL_Texture* curTexture;
	
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[12][0]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 0;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}


void Intro::RenderFlame()
{
	m_curDelayFlame += m_tickElapse;
	if (FLAME_DELAY < m_curDelayFlame)
	{
		m_curDelayFlame %= FLAME_DELAY;
		m_curFlame++;
		m_curFlame %= 4;
	}

	float width;
	float height;
	int window_width, window_height;
	SDL_Texture* curTexture;

	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	height = static_cast<float>(m_resources->m_Image16FileData[12][static_cast<size_t>(m_curFlame + 1)].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][static_cast<size_t>(m_curFlame + 1)].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[12][static_cast<size_t>(m_curFlame + 1)]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();

	switch (m_curMode)
	{
	case IntroMode::FADE_LOGO:
		RenderFadeIn();
		break;
	case IntroMode::FADE_FLAME:
		break;
	default:
		RenderLogo();
		RenderFlame();
		break;
	}

	m_sdl_helper->RenderPresent();
}

void Intro::LoadData()
{
	SetSDLData();
}

void Intro::SetSDLData()
{
	GameObject::SetSDLData();
}

