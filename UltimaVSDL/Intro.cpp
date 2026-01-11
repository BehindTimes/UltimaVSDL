#include "Intro.h"

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources)
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

	height = static_cast<float>(m_resources->m_Image16FileData[2][1].height);
	width = static_cast<float>(m_resources->m_Image16FileData[2][1].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[2][1]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 66;
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

	m_sdl_helper->RenderPresent();
}
