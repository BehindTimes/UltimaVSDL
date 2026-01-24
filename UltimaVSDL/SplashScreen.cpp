#include "SplashScreen.h"
#include "ColorData.h"
#include "SDL3Helper.h"
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include <cstring>
#include "U5Input.h"
#include <memory>

extern std::unique_ptr<U5Input> m_input;

SplashScreen::SplashScreen(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_CurrentLogo(0),
	m_curDelay(0),
	m_curMode(SplashScreenMode::LOGO),
	m_oldMode(SplashScreenMode::LOGO)
{
	m_clearScreen = true;
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::RenderLogo()
{

	m_curDelay += m_tickElapse;
	if (m_curDelay > LOGO_ROTATE_DELAY && m_CurrentLogo < 7)
	{
		m_CurrentLogo++;
		m_curDelay = 0;
	}
	else if(m_curDelay > LOGO_ROTATE_DELAY + LOGO_DELAY && m_CurrentLogo == 7)
	{
		m_CurrentLogo++;
		m_curDelay = 0;
	}

	m_sdl_helper->ClearScreen();

	if (m_CurrentLogo > 7)
	{
		m_curMode = SplashScreenMode::SIGNATURE;
	}
	else if (m_CurrentLogo > 1)
	{
		float width;
		float height;
		float x = 0;
		float y = 0;
		float scale = 1.0f;
		int window_width, window_height;
		SDL_Texture *curTexture;

		m_sdl_helper->GetScreenDimensions(window_width, window_height);
		float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
		float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

		if (m_CurrentLogo < 8)
		{
			scale = static_cast<float>(m_curDelay) / (LOGO_ROTATE_DELAY / 2.0f);
			if (scale > 1.0f)
			{
				if (m_CurrentLogo < 7)
				{
					scale = 1.0f - (scale - 1.0f);
				}
				else
				{
					x = 108;
					y = 140;
					scale = 1.0f;
					curTexture = m_sdl_helper->m_BitFileTextures[1][7];
					height = static_cast<float>(m_resources->m_BitFileData[1][7].height) * scale;
					width = static_cast<float>(m_resources->m_BitFileData[1][7].width);
					m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
				}
			}
		}

		height = static_cast<float>(m_resources->m_BitFileData[1][static_cast<size_t>(m_CurrentLogo - 1)].height) * scale;
		width = static_cast<float>(m_resources->m_BitFileData[1][static_cast<size_t>(m_CurrentLogo - 1)].width);
		
		x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
		y = (ORIGINAL_GAME_HEIGHT - height) / 2.0f - 23;

		curTexture = m_sdl_helper->m_BitFileTextures[1][static_cast<size_t>(m_CurrentLogo - 1)];

		int curColor = 9;
		if (m_resources->m_render_mode == RenderMode::CGA)
		{
			curColor = 1;
			SDL_SetTextureColorMod(curTexture, cga_table[curColor][0], cga_table[curColor][1], cga_table[curColor][2]);
		}
		else
		{
			SDL_SetTextureColorMod(curTexture, ega_table[curColor][0], ega_table[curColor][1], ega_table[curColor][2]);
		}

		m_sdl_helper->RenderFlipTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult, (m_CurrentLogo + 1) % 2);
	}
}

void SplashScreen::RenderSignature()
{
	m_curDelay += m_tickElapse;
	float width;
	float height;
	int window_width, window_height;
	SDL_Texture *curTexture;

	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_BitFileTextures[1][8]; // A

	height = static_cast<float>(m_resources->m_BitFileData[1][8].height);
	width = static_cast<float>(m_resources->m_BitFileData[1][8].width);

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
	
	if (m_curDelay > FULL_SIG_DISPLAY) // Just display everything
	{
		curTexture = m_sdl_helper->m_BitFileTextures[0][0]; // Lord British
		height = static_cast<float>(m_resources->m_BitFileData[0][0].height);
		width = static_cast<float>(m_resources->m_BitFileData[0][0].width);

		x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
		y = 66;
		m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);

		curTexture = m_sdl_helper->m_BitFileTextures[1][9]; // Production
		height = static_cast<float>(m_resources->m_BitFileData[1][9].height);
		width = static_cast<float>(m_resources->m_BitFileData[1][9].width);

		x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
		y = 160;
		m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);

		if (m_curDelay > FULL_SIG_DISPLAY_DONE) // We're done here
		{
			m_newMode = U5Modes::Menu;
		}
	}
	else if (m_curDelay > A_DELAY) // Draw the signature
	{
		curTexture = m_sdl_helper->m_PathFileTexture; // Lord British
		height = 62;
		width = 272;

		float percentToDraw = (m_curDelay - A_DELAY) / static_cast<float>(FULL_SIG_DISPLAY - A_DELAY);
		if (0 != LoadSignaturePath(percentToDraw))
		{
			m_curDelay = FULL_SIG_DISPLAY;
			m_clearScreen = true;
		}
		x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
		y = 66;
		m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
	}
}

int SplashScreen::LoadSignaturePath(float percent)
{
	size_t maxPixel = static_cast<size_t>(m_resources->m_PathFileData.size() * percent);
	const int offsetX = 20;
	const int offsetY = 2;

	int xpos = offsetX;
	int ypos = offsetY;
	int zerocount = 0;

	unsigned char* pixels = NULL;
	int pitch;
	SDL_LockTexture(m_sdl_helper->m_PathFileTexture, NULL, (void**)&pixels, &pitch);
	std::memset(pixels + (pitch * ypos) + (xpos * 4), 0xFF, sizeof(unsigned char) * 4);
	for (size_t index = 0; index < maxPixel; index++)
	{
		unsigned char curByte = m_resources->m_PathFileData[index];
		if (curByte != 0)
		{
			unsigned char leftright = (unsigned char)((curByte >> 4) & 0xF);
			unsigned char updown = (unsigned char)(curByte & 0xF);

			if (leftright >= 8)
			{
				int subval = leftright - 8;
				xpos -= subval;
			}
			else
			{
				xpos += leftright;
			}
			if (updown >= 8)
			{
				int subval = updown - 8;
				ypos -= subval;
			}
			else
			{
				ypos += updown;
			}

			if ((updown >= 3 && updown < 8) || (updown >= 0x0a) || (leftright >= 3 && leftright < 8) || (leftright >= 0x0a)) // d5
			{
				continue;
			}
			else
			{
				if (xpos >= 0 && xpos < 272 && ypos >= 0 && ypos < 62)
				{
					memset(pixels + (pitch * ypos) + (xpos * 4), 0xFF, sizeof(unsigned char) * 4);
				}
			}
		}
		else
		{
			zerocount++;
			switch (zerocount)
			{
			case 1:
				xpos = offsetX + 20;
				ypos = offsetY + 26;
				break;
			case 2:
				xpos = offsetX + 99;
				ypos = offsetY + 10;
				break;
			case 3:
				xpos = offsetX + 123;
				ypos = offsetY + 37;
				break;
			default:
				SDL_UnlockTexture(m_sdl_helper->m_PathFileTexture);
				return - 1;
			}

			if (xpos >= 0 && xpos < 272 && ypos >= 0 && ypos < 62)
			{
				memset(pixels + (pitch * ypos) + (xpos * 4), 0xFF, sizeof(unsigned char) * 4);
			}
		}
	}
	SDL_UnlockTexture(m_sdl_helper->m_PathFileTexture);
	return 0;
}

void SplashScreen::Render()
{
	if (m_oldMode != m_curMode)
	{
		m_oldMode = m_curMode;
		m_sdl_helper->ClearScreen();
		m_sdl_helper->RenderPresent();
		m_curDelay = 0;
	}
	else
	{
		m_sdl_helper->ClearScreen();

		switch (m_curMode)
		{
		case SplashScreenMode::LOGO:
			RenderLogo();
			break;
		case SplashScreenMode::SIGNATURE:
			RenderSignature();
			break;
		default:
			break;
		}
	}
	m_sdl_helper->RenderPresent();
}

void SplashScreen::LoadData()
{
	SetSDLData();
	m_input->SetInputType(InputType::ANY_KEY);
}

void SplashScreen::SetSDLData()
{
	GameObject::SetSDLData();
}

void SplashScreen::ProcessEvents()
{
	if (m_input->isAnyKeyHit())
	{
		m_newMode = U5Modes::MenuSkip;
	}
}
