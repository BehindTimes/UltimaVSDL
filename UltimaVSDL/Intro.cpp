#include "Intro.h"
#include "U5Utils.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include "ColorData.h"
#include <SDL3/SDL_rect.h>

extern std::unique_ptr<U5Utils> m_utilities;
static const int NUM_STORY = 21;

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0),
	m_curMode(IntroMode::FADE_LOGO),
	m_window_width(0),
	m_window_height(0),
	m_num_pixels(0),
	m_num_flame1_pixels(0),
	m_num_wod_pixels(0),
	m_curLogoFade(0),
	m_curFlame1Fade(0),
	m_curWodFade(0),
	m_curStoryboard(0)
{
	m_clearScreen = true;

	m_curMode = IntroMode::STORY;

	m_logo_fade_count = static_cast<int>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width);
	m_cur_logo_count = m_logo_fade_count;
	m_logo_fade_locations.resize(static_cast<size_t>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width));

	m_flame1_fade_count = static_cast<int>(m_resources->m_Image16FileData[12][1].height * m_resources->m_Image16FileData[12][1].width);
	m_cur_flame1_fade_count = m_flame1_fade_count;
	m_flame1_fade_locations.resize(static_cast<size_t>(m_resources->m_Image16FileData[12][1].height * m_resources->m_Image16FileData[12][1].width));

	m_wod_fade_count = static_cast<int>(m_resources->m_BitFileData[2][0].height * m_resources->m_BitFileData[2][0].width);
	m_cur_wod_fade_count = m_wod_fade_count;
	m_wod_fade_locations.resize(static_cast<size_t>(m_resources->m_BitFileData[2][0].height * m_resources->m_BitFileData[2][0].width));
}

Intro::~Intro()
{
}

void Intro::RenderWoD()
{
	float width;
	float height;

	SDL_Texture* curTexture;

	height = static_cast<float>(m_resources->m_BitFileData[2][0].height);
	width = static_cast<float>(m_resources->m_BitFileData[2][0].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_WoDFadeTexture;

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderFlameFadeWoD()
{
	float width;
	float height;

	SDL_Texture* curTexture;
	uint32_t num_pixels = 0;

	m_curWodFade += m_tickElapse;

	height = static_cast<float>(m_resources->m_BitFileData[2][0].height);
	width = static_cast<float>(m_resources->m_BitFileData[2][0].width);

	if (m_curWodFade > WOD_FADE_DELAY)
	{
		m_curMode = IntroMode::SHOW_ALL;
	}
	else
	{
		uint32_t total_pixels = static_cast<uint32_t>(((m_curWodFade) / static_cast<float>(WOD_FADE_DELAY)) * m_cur_wod_fade_count);
		num_pixels = total_pixels - m_num_wod_pixels;
		m_num_wod_pixels = total_pixels;

		if (m_wod_fade_count == 0)
		{
			return;
		}
	}

	std::vector<int> vec_on;
	curTexture = m_sdl_helper->m_WoDFadeTexture;

	// Set the off pixels now
	for (uint32_t index = 0; index < num_pixels; index++)
	{
		uint32_t curOnPixel = m_utilities->GetRandom(0, m_wod_fade_count);
		if (m_wod_fade_count > 0)
		{
			m_wod_fade_count--;
		}
		else
		{
			break;
		}
		uint32_t cur_vector_index = 0;

		for (size_t vector_index = 0; vector_index < m_wod_fade_locations.size(); vector_index++)
		{
			if (m_wod_fade_locations[vector_index] != 0)
			{
				continue;
			}
			if (cur_vector_index == curOnPixel)
			{
				m_wod_fade_locations[vector_index] = 1;
				vec_on.push_back(static_cast<int>(vector_index));
				break;
			}
			cur_vector_index++;
		}
	}
	if (vec_on.size() > 0)
	{
		m_sdl_helper->TurnOnPixels(curTexture, vec_on, false);
	}
}

void Intro::RenderFlameFadeIn1()
{
	float width;
	float height;

	SDL_Texture* curTexture;
	uint32_t num_pixels = 0;

	m_curFlame1Fade += m_tickElapse;

	height = static_cast<float>(m_resources->m_Image16FileData[12][1].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][1].width);

	if (m_curFlame1Fade > FLAME_FADE_1_DELAY)
	{
		m_curMode = IntroMode::FADE_FLAME_2;
	}
	else
	{
		uint32_t total_pixels = static_cast<uint32_t>(((m_curFlame1Fade) / static_cast<float>(FLAME_FADE_1_DELAY)) * m_cur_wod_fade_count);
		num_pixels = total_pixels - m_num_flame1_pixels;
		m_num_flame1_pixels = total_pixels;

		if (m_flame1_fade_count == 0)
		{
			return;
		}
	}

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_Flame1FadeTexture;
	std::vector<int> vec_on;

	// Set the on pixels now
	for (uint32_t index = 0; index < num_pixels; index++)
	{
		uint32_t curOnPixel = m_utilities->GetRandom(0, m_flame1_fade_count);
		if (m_flame1_fade_count > 0)
		{
			m_flame1_fade_count--;
		}
		else
		{
			break;
		}
		uint32_t cur_vector_index = 0;

		for (size_t vector_index = 0; vector_index < m_flame1_fade_locations.size(); vector_index++)
		{
			if (m_flame1_fade_locations[vector_index] != 0)
			{
				continue;
			}
			if (cur_vector_index == curOnPixel)
			{
				m_flame1_fade_locations[vector_index] = 1;
				vec_on.push_back(static_cast<int>(vector_index));
				break;
			}
			cur_vector_index++;
		}
	}
	if (vec_on.size() > 0)
	{
		m_sdl_helper->TurnOnPixels(curTexture, vec_on, false);
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderLogoFadeIn()
{
	float width;
	float height;

	SDL_Texture* curTexture;
	uint32_t num_pixels = 0;

	m_curLogoFade += m_tickElapse;

	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	if (m_curLogoFade > LOGO_FADE_DELAY)
	{
		m_curMode = IntroMode::FADE_FLAME_1;
		RenderLogo();
		return;
	}
	else
	{
		uint32_t total_pixels = static_cast<uint32_t>(((m_curLogoFade) / static_cast<float>(LOGO_FADE_DELAY)) * m_logo_fade_count);
		num_pixels = total_pixels - m_num_pixels;
		m_num_pixels = total_pixels;

		if (m_cur_logo_count == 0)
		{
			m_num_pixels = 0;
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
		m_sdl_helper->TurnOnPixels(curTexture, vec_on, true);
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

void Intro::RenderIntroBox()
{
	const int blue = 1;

	float x = 0;
	float y = 15 * HALF_TILE_HEIGHT;
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX];

	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH, TILE_HEIGHT * 5);
	x = HALF_TILE_WIDTH;
	y = 8 * TILE_HEIGHT;
	curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY];
	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH - TILE_WIDTH, TILE_HEIGHT * 4);

	SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][123], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
	SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][124], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
	SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][125], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
	SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][126], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);

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

void Intro::RenderStory()
{
	const int NUM_STORY_DATA = 6;
	if (m_curStoryboard >= NUM_STORY || m_resources->m_data.story_text.size() != NUM_STORY)
	{
		m_curMode = IntroMode::SHOW_ALL;
		return;
	}
	U5StoryText& curData = m_resources->m_data.story_text[m_curStoryboard];
	if (curData.story_number >= NUM_STORY_DATA || curData.story_number >= m_sdl_helper->m_Image16FileTextures.size())
	{
		return;
	}
	auto& curStoryImages = m_resources->m_Image16FileData[curData.story_number + IV16_STORY1];
	auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[curData.story_number + IV16_STORY1];
	
	if (curData.image_index >= curStoryImages.size() || curData.image_index >= curStoryTextures.size())
	{
		return;
	}
	auto& curImageData = curStoryImages[curData.image_index];
	auto& curImageTexture = curStoryTextures[curData.image_index];

	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	m_sdl_helper->RenderTextureAt(curImageTexture, curData.picture_x * hMult, curData.picture_y * vMult, curImageData.width * hMult, curImageData.height * vMult);

	if (curData.action == 4)
	{
		if (3 >= curStoryImages.size() || 3 >= curStoryTextures.size())
		{
			return;
		}
		auto& tempImageData = curStoryImages[3];
		auto& tempImageTexture = curStoryTextures[3];
		float tempx = curData.picture_x * hMult;
		float tempy = (curData.picture_y + curImageData.height) * hMult;
		m_sdl_helper->RenderTextureAt(tempImageTexture, tempx, tempy, tempImageData.width * hMult, tempImageData.height * vMult);
	}
	else if (curData.action == 5)
	{
		if (5 >= curStoryImages.size() || 5 >= curStoryTextures.size())
		{
			return;
		}
		auto& tempImageTexture = curStoryTextures[5];
		auto& tempImageData = curStoryImages[5];
		float tempx = curData.picture_x * hMult;
		float tempy = (curData.picture_y + curImageData.height) * hMult;
		m_sdl_helper->RenderTextureAt(tempImageTexture, tempx, tempy, tempImageData.width * hMult, tempImageData.height * vMult);
	}
	if (curData.action == 6)
	{
		if (5 >= curStoryImages.size() || 7 >= curStoryTextures.size())
		{
			return;
		}
		auto& tempImageTexture = curStoryTextures[7];
		auto& tempImageData = curStoryImages[7];
		float tempx = curData.picture_x * hMult;
		float tempy = (curData.picture_y + curImageData.height) * hMult;
		m_sdl_helper->RenderTextureAt(tempImageTexture, tempx, tempy, tempImageData.width * hMult, tempImageData.height * vMult);
	}
}

void Intro::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();

	switch (m_curMode)
	{
	case IntroMode::STORY:
		RenderStory();
		break;
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
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, ega_table[blue][0], ega_table[blue][1], ega_table[blue][2], 0xFF);
	//SDL_Texture* curTexture = m_sdl_helper->m_BitFileTextures[TTV_INTROBOX];

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
	case IntroMode::STORY:
		if (m_sdl_helper->isAnyKeyHit())
		{
			IncrementStory();
		}
		break;
	case IntroMode::FADE_LOGO:
	case IntroMode::FADE_FLAME_1:
	case IntroMode::FADE_FLAME_2:
		if (m_sdl_helper->isAnyKeyHit())
		{
			m_newMode = U5Modes::MenuSkip;
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

void Intro::IncrementStory()
{
	m_curStoryboard++;
	if (m_curStoryboard >= NUM_STORY)
	{
		m_curStoryboard = 0;
		m_curMode = IntroMode::SHOW_ALL;
	}
}
