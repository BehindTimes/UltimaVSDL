#include "Intro.h"

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0)
{
	m_clearScreen = true;
}

Intro::~Intro()
{
}

void Intro::RenderFadeIn()
{
	float width;
	float height;
	int window_width, window_height;
	SDL_Texture* curTexture;

	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

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

	height = static_cast<float>(m_resources->m_Image16FileData[12][m_curFlame + 1].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][m_curFlame + 1].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[12][m_curFlame + 1]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::Render()
{
	//if (m_clearScreen)
	{
		m_sdl_helper->ClearScreen();
		m_clearScreen = false;
	}

	RenderFadeIn();
	RenderFlame();

	m_sdl_helper->RenderPresent();
}
