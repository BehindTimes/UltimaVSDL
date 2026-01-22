#include "Intro.h"
#include "U5Utils.h"
#include <memory>
#include <vector>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include "ColorData.h"
#include <SDL3/SDL_rect.h>
#include "FadeObject.h"
#include "CutScene.h"
#include <functional>

extern std::unique_ptr<CutScene> cutscene_screen;
extern std::unique_ptr<U5Utils> m_utilities;

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0),
	m_curMode(IntroMode::FADE_LOGO),
	m_window_width(0),
	m_window_height(0),
	m_curWodFade(0)
{
	m_clearScreen = true;

	m_LogoFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_LogoFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width));
	m_LogoFade->SetDelay(LOGO_FADE_DELAY);

	m_FlameFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_FlameFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][1].height * m_resources->m_Image16FileData[12][1].width));
	m_FlameFade->SetDelay(FLAME_FADE_DELAY);

	m_WoDFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_WoDFade->SetSize(static_cast<size_t>(m_resources->m_BitFileData[2][0].height * m_resources->m_BitFileData[2][0].width));
	m_WoDFade->SetDelay(WOD_FADE_DELAY);	
}

Intro::~Intro()
{
}

void Intro::RenderWoD()
{
	float CGA_OFFSET = 0;
	float width;
	float height;

	SDL_Texture* curTexture;

	// Not sure why the Warriors of Destiny image is shifted 4 pixels over, but it is
	if (m_resources->m_render_mode == RenderMode::CGA)
	{
		CGA_OFFSET = -4;
	}

	height = static_cast<float>(m_resources->m_BitFileData[2][0].height);
	width = static_cast<float>(m_resources->m_BitFileData[2][0].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_WoDFadeTexture;

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, (x + CGA_OFFSET) * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderFlameFadeWoD()
{
	SDL_Texture* curTexture = m_sdl_helper->m_WoDFadeTexture;

	m_WoDFade->AddElapsedTime(m_tickElapse);
	if (!m_WoDFade->IsFading())
	{
		m_curMode = IntroMode::SHOW_ALL;
	}
	m_WoDFade->ProcessFade(curTexture, false);
}

void Intro::RenderFlameFadeIn1()
{
	float width;
	float height;
	height = static_cast<float>(m_resources->m_Image16FileData[12][1].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][1].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	SDL_Texture* curTexture = m_sdl_helper->m_Flame1FadeTexture;

	m_FlameFade->AddElapsedTime(m_tickElapse);
	if (!m_FlameFade->IsFading())
	{
		m_curMode = IntroMode::FADE_FLAME_2;
	}
	bool done = m_FlameFade->ProcessFade(curTexture, false);
	if (done)
	{
		return;
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 64;

	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderLogoFadeIn()
{
	float width;
	float height;
	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	SDL_Texture* curTexture = m_sdl_helper->m_LogoFadeTexture;

	m_LogoFade->AddElapsedTime(m_tickElapse);
	if (!m_LogoFade->IsFading())
	{
		m_curMode = IntroMode::FADE_FLAME_1;
		RenderLogo();
		return;
	}
	bool done = m_LogoFade->ProcessFade(curTexture);
	if (done)
	{
		RenderLogo();
		return;
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

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

void Intro::RenderIntroBox()
{
	int blue = 1;

	float x = 0;
	float y = 15 * HALF_TILE_HEIGHT;
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX];

	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH, TILE_HEIGHT * 5);
	x = HALF_TILE_WIDTH;
	y = 8 * TILE_HEIGHT;
	curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY];
	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH - TILE_WIDTH, TILE_HEIGHT * 4);

	if (m_resources->m_render_mode == RenderMode::CGA) // CGA
	{
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][123], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][124], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][125], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][126], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
	}
	else
	{
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][123], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][124], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][125], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][126], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
	}

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX]);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][123];
	m_sdl_helper->RenderTextureAt(curTexture, 0, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][124];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][125];
	m_sdl_helper->RenderTextureAt(curTexture, 0, HALF_TILE_HEIGHT * 9, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][126];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, HALF_TILE_HEIGHT * 9, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);

	//curTexture = m_sdl_helper->m_ArrowTextures[1];
	//m_sdl_helper->RenderTextureAt(curTexture, 100, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_FRect line{ HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, RENDER_WIDTH - TILE_WIDTH + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, (TILE_HEIGHT * 4) + HALF_TILE_HEIGHT, RENDER_WIDTH - TILE_WIDTH + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT, LINE_THICKNESS, (TILE_HEIGHT * 4) };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { RENDER_WIDTH - HALF_TILE_WIDTH, HALF_TILE_HEIGHT, LINE_THICKNESS, (TILE_HEIGHT * 4) };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	

	m_sdl_helper->SetRenderTarget(nullptr);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}

void Intro::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();

	switch (m_curMode)
	{
	case IntroMode::FADE_LOGO:
		RenderLogoFadeIn();
		break;
	case IntroMode::FADE_FLAME_1:
		RenderLogo();
		RenderFlame();
		RenderFlameFadeIn1();
		RenderWoD();
		break;
	case IntroMode::FADE_FLAME_2:
		RenderLogo();
		RenderFlame();
		RenderFlameFadeWoD();
		RenderWoD();
		break;
	default:
		RenderLogo();
		RenderFlame();
		RenderIntroBox();
		break;
	}

	m_sdl_helper->RenderPresent();
}

void Intro::LoadData()
{
	SetSDLData();
	CreateIntroBox();
}

void Intro::SetSDLData()
{
	GameObject::SetSDLData();
}

void Intro::CreateIntroBox()
{
	const int blue = 1;
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX]);
	if (m_resources->m_render_mode == RenderMode::CGA) // CGA
	{
		SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, cga_table[blue][0], cga_table[blue][1], cga_table[blue][2], 0xFF);
	}
	else
	{
		SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, ega_table[blue][0], ega_table[blue][1], ega_table[blue][2], 0xFF);
	}

	m_sdl_helper->ClearScreen();
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0x00, 0, 0x00, 0xFF);
	m_sdl_helper->ClearScreen();
	m_sdl_helper->SetRenderTarget(nullptr);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}

void Intro::ProcessEvents()
{
	switch (m_curMode)
	{
	case IntroMode::FADE_LOGO:
	case IntroMode::FADE_FLAME_1:
	case IntroMode::FADE_FLAME_2:
		if (m_sdl_helper->isAnyKeyHit())
		{
			m_newMode = U5Modes::MenuSkip;
		}
		break;
	case IntroMode::SHOW_ALL:
		if (m_sdl_helper->isAnyKeyHit())
		{
			m_newMode = U5Modes::Cutscene;
		}
		break;
	default:
		break;
	}
}

void Intro::GoToSelection()
{
	m_curMode = IntroMode::SHOW_ALL;
}

bool Intro::ChangeMode(U5Modes& newMode)
{
	bool ret = GameObject::ChangeMode(newMode);

	if (newMode == U5Modes::Cutscene)
	{
		cutscene_screen->SetCutScreenInfo(U5Modes::MenuSkip, m_resources->m_data.story_text, IV16_STORY1, std::bind(&Intro::StoryOverCallback, this));
		//cutscene_screen->SetCutScreenInfo(U5Modes::MenuSkip, m_resources->m_data.ending_text, IV16_END1, std::bind(&Intro::StoryOverCallback, this));
	}

	return ret;
}

void Intro::StoryOverCallback()
{
	m_curMode = IntroMode::SHOW_ALL;
}
