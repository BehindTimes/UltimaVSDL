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
#include <iostream>
#include <string>
#include "FadeObject.h"

extern std::unique_ptr<U5Utils> m_utilities;
static const int NUM_STORY = 21;

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0),
	m_curMode(IntroMode::FADE_LOGO),
	m_window_width(0),
	m_window_height(0),
	m_curWodFade(0),
	m_curStoryboard(0)
{
	m_clearScreen = true;

	//m_curMode = IntroMode::STORY;

	m_LogoFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_LogoFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width));
	m_LogoFade->SetDelay(LOGO_FADE_DELAY);

	m_FlameFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_FlameFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][1].height * m_resources->m_Image16FileData[12][1].width));
	m_FlameFade->SetDelay(FLAME_FADE_DELAY);

	m_WoDFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_WoDFade->SetSize(static_cast<size_t>(m_resources->m_BitFileData[2][0].height * m_resources->m_BitFileData[2][0].width));
	m_WoDFade->SetDelay(WOD_FADE_DELAY);

	m_CodexFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_CodexFade->SetSize(static_cast<size_t>(CODEX_WIDTH * CODEX_HEIGHT));
	m_CodexFade->SetDelay(CODEX_FADE_DELAY);
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
	U5StoryScreen& curData = m_resources->m_data.story_text[m_curStoryboard];
	if (curData.story_number >= NUM_STORY_DATA || curData.story_number >= m_sdl_helper->m_Image16FileTextures.size())
	{
		return;
	}
	auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number + IV16_STORY1)];
	auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number + IV16_STORY1)];
	
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

	// This is case specific handling
	if (curData.action == 3)
	{
		if (curStoryImages.size() > 3)
		{
			const float x_offset = 24;
			const float y_offset = 1;
			auto& curImageData1 = curStoryImages[3];
			auto& curImageTexture1 = curStoryTextures[3];
			m_sdl_helper->RenderTextureAt(curImageTexture1, (curData.picture_x + x_offset) * hMult,
				(curData.picture_y + y_offset) * vMult, curImageData1.width * hMult, curImageData1.height * vMult);
		}
	}

	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_FullScreenTexture, 0, 0, RENDER_WIDTH, RENDER_HEIGHT);

	if (curData.action == 1)
	{
		float title_x_pos[2] = { 0,0 };
		float title_y_pos[2] = { 0,0 };
		SDL_Texture* title_texture[2] = { nullptr, nullptr };
		U5ImageData title_data[2];

		switch (curData.story_number)
		{
		case 0:
			title_x_pos[0] = 224;
			title_y_pos[0] = 30;
			title_x_pos[1] = 168;
			title_y_pos[1] = 58;
			title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
			title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][1];
			title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
			title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][1];
			break;
		case 2:
			title_x_pos[0] = 232;
			title_y_pos[0] = 26;
			title_x_pos[1] = 200;
			title_y_pos[1] = 54;
			title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
			title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][2];
			title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
			title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][2];
			break;
		case 5:
			title_x_pos[0] = 184;
			title_y_pos[0] = 0;
			title_x_pos[1] = 248;
			title_y_pos[1] = 0;
			title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
			title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][3];
			title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
			title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][3];
			break;
		default:
			break;
		}
		if (title_texture[0] != nullptr && title_texture[1] != nullptr)
		{
			m_sdl_helper->RenderTextureAt(title_texture[0], title_x_pos[0] * hMult, title_y_pos[0] * vMult,
				title_data[0].width * hMult, title_data[0].height * vMult);
			m_sdl_helper->RenderTextureAt(title_texture[1], title_x_pos[1] * hMult, title_y_pos[1] * vMult,
				title_data[1].width * hMult, title_data[1].height * vMult);
		}
	}
	else if (curData.action == 2)
	{
		float tempx = 40 * hMult;
		float tempy = 86 * hMult;

		m_CodexFade->AddElapsedTime(m_tickElapse, m_sdl_helper->m_CodexFadeTexture, true);
		bool isfading = m_CodexFade->IsFading();
		if (isfading)
		{
			m_CodexFade->ProcessFade(m_sdl_helper->m_CodexFadeTexture);
		}
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_CodexFadeTexture, tempx, tempy, CODEX_WIDTH * hMult, CODEX_HEIGHT * vMult);
	}
	else if (curData.action == 4)
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
	else if (curData.action == 6)
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
	RenderStoryTexture();
	CreateStreamingTextures();
}

void Intro::SetSDLData()
{
	GameObject::SetSDLData();
}

void Intro::CreateStreamingTextures()
{
	m_CodexFade->Reset();
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
	case IntroMode::SHOW_ALL:
		if (m_sdl_helper->isAnyKeyHit())
		{
			m_sdl_helper->TurnOnAllPixels(m_sdl_helper->m_CodexFadeTexture, false);
			m_CodexFade->Reset();
			m_curMode = IntroMode::STORY;
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
	RenderStoryTexture();
}

// The game wants full words, and will space those words appropriately.
// That is unless an _ is found, at which point, if that still fits, it will cut off at that and use a -.
int Intro::GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string &str_out,
	int &num_spaces, int &final_size)
{
	const int SPACE_LEN = 5;
	auto& dash_letter = m_resources->m_ProportionalFontData[13];
	int dash_len = dash_letter.real_width;
	const int TAB_LEN = 15;
	int max_len = right - left;
	max_len--;
	num_spaces = 0;
	final_size = 0;
	if (max_len <= 0)
	{
		return -1;
	}
	int curlen = 0;
	for (size_t index = start_word; index < letter_list.size(); index++)
	{
		unsigned char temp_letter = letter_list[index];
		int temp_len = 0;
		if (temp_letter == 0x7b)
		{
			temp_len = TAB_LEN - 1;
		}
		else if (temp_letter == 0x0A) // newline
		{
			final_size = -1;
			return static_cast<int>(index + 1);
		}
		else if (temp_letter == 0x20) // space
		{
			temp_len = SPACE_LEN - 1;
		}
		else if (temp_letter == 0x5F) // underscore
		{
			temp_len = -1;
		}
		else if (temp_letter > 0x20 && temp_letter <= 0x7a) // ASCII
		{
			auto& curLetter = m_resources->m_ProportionalFontData[static_cast<size_t>(temp_letter - 0x20)];
			temp_len = curLetter.real_width;
		}
		else
		{
			return -1; // ???
		}
		// TO DO: DO STUFF HERE
		if (curlen + temp_len >= max_len)
		{
			// Sanity check
			if (index < 2)
			{
				return -1;
			}

			if (temp_letter == ' ')
			{
				final_size = curlen;
				return static_cast<int>(index + 1);
			}
			else
			{
				// Start popping the last letter until you reach a space or underscore.
				int temp_index = static_cast<int>(index);
				do
				{
					unsigned char back_char = str_out.back();
					str_out.pop_back();
					temp_index--;
					// We're done here
					if (back_char == ' ')
					{
						curlen -= SPACE_LEN;
						final_size = curlen;
						num_spaces--;
						temp_index++;
						return temp_index;
					}
					else if (back_char == '_')
					{
						
						if (curlen + dash_len < max_len)
						{
							str_out += "_";
							curlen += dash_len;
							final_size = curlen;
							return temp_index + 1;
						}	
					}
					else if (back_char > 0x20 && back_char <= 0x7a) // ASCII
					{
						auto& curLetter = m_resources->m_ProportionalFontData[static_cast<size_t>(back_char - 0x20)];
						curlen -= (curLetter.real_width + 1);
					}
					
				} while (!str_out.empty());
			}
		}

		curlen += (temp_len + 1);
		str_out += temp_letter;
		if (temp_letter == ' ')
		{
			num_spaces++;
		}
	}
	return 0;
}

void Intro::RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size)
{
	// We always end on a letter, so remove the letter spacing of the very last letter
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	const size_t DASH_POS = 13;
	const int SPACE_LEN = 5;
	const int TAB_LEN = 15;
	int total_size = x_right - x_left;
	int space_size = 0;
	
	int remainder = 0;
	if (num_spaces > 0 && final_size > 0)
	{
		space_size = (total_size - final_size) / num_spaces;
		remainder = (total_size - final_size) % num_spaces;
	}

	int cur_left = x_left;
	
	for (size_t index = 0; index < str_line.size(); index++)
	{
		if (str_line[index] == 0x7b)
		{
			cur_left += TAB_LEN;
		}
		else if (str_line[index] == ' ')
		{
			cur_left += SPACE_LEN + space_size + (remainder > 0 ? 1 : 0);
			if (remainder > 0)
			{
				remainder--;
			}
		}
		else if (str_line[index] > 0x20)
		{
			size_t letter_pos = static_cast<size_t>(str_line[index] - 0x20);
			if (str_line[index] == '_')
			{
				if (index != str_line.size() - 1)
				{
					continue;
				}
				letter_pos = DASH_POS;
			}
			
			auto& curLetter = m_resources->m_ProportionalFontData[letter_pos];
			m_sdl_helper->RenderTextureAt(m_sdl_helper->m_ProportionalFontTextures[letter_pos],
				cur_left * hMult, y_pos * vMult, curLetter.width * hMult, curLetter.height * vMult);
			cur_left += curLetter.real_width + 1;
		}
	}
}

void Intro::RenderStoryTexture()
{
	const int LINE_HEIGHT = 9;
	const size_t ANKH_LOCATION = 2;

	auto& curData = m_resources->m_data.story_text[m_curStoryboard];

	// This is a very specific case, where it will copy 45x34 of the third texture to fade in
	// Now, this is only done in one place in the game, but let's stay compliant to the original engine
	if (curData.action == 2)
	{
		if (m_resources->m_Image16FileData[static_cast<size_t>(IV16_STORY1 + curData.story_number)].size() > (ANKH_LOCATION) &&
			m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(IV16_STORY1 + curData.story_number)].size() > (ANKH_LOCATION))
		{
			auto& curTexture = m_resources->m_Image16FileData[static_cast<size_t>(IV16_STORY1 + curData.story_number)][ANKH_LOCATION];
			if (curTexture.width >= static_cast<uint32_t>(CODEX_WIDTH) && curTexture.height >= static_cast<uint32_t>(CODEX_HEIGHT))
			{
				m_sdl_helper->CopyTextureToStreaming(curTexture,
					m_sdl_helper->m_CodexFadeTexture, CODEX_WIDTH, CODEX_HEIGHT);
			}
		}
		//
	}

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_FullScreenTexture);
	m_sdl_helper->ClearScreen();

	// This is a special case
	if (curData.action == 3)
	{
		std::string strLine1(m_resources->m_data.intro_string_1.begin(), m_resources->m_data.intro_string_1.end());
		RenderIntroLine(32, 47, 9, strLine1, 0, 0);
		std::string strLine2(m_resources->m_data.intro_string_2.begin(), m_resources->m_data.intro_string_2.end());
		RenderIntroLine(32, 47, 180, strLine2, 0, 0);
		m_sdl_helper->SetRenderTarget(nullptr);
		return;
	}

	int ypos = curData.text_y_pos;

	// Render the first sentence
	int x_left = curData.first_line_offset;
	int x_right = curData.paragraph[0].text_right_pos;
	std::string text_line;

	int num_spaces = 0;
	int final_size = 0;
	int new_text_pos = GetLine(x_left, x_right, 0, curData.text, text_line, num_spaces, final_size);
	// Sanity check
	if (new_text_pos < 0)
	{
		return;
	}
	else if (new_text_pos == 0)
	{
		final_size = 0;
	}

	RenderIntroLine(x_left, x_right, ypos, text_line, num_spaces, final_size);
	ypos += LINE_HEIGHT;

	int curParagraph = 0;

	while (final_size != 0)
	{
		std::string new_text_line;
		if (curParagraph == 0)
		{
			x_left = curData.paragraph[0].text_left_pos;
			x_right = curData.paragraph[0].text_right_pos;
		}
		else if (curParagraph == 1)
		{
			x_left = curData.paragraph[1].text_left_pos;
			x_right = curData.paragraph[1].text_right_pos;
		}
		else
		{
			x_left = 0;
			x_right = 320;
		}
		new_text_pos = GetLine(x_left, x_right, new_text_pos, curData.text, new_text_line, num_spaces, final_size);
		if (new_text_pos < 0)
		{
			break;
		}
		else if (new_text_pos == 0)
		{
			final_size = 0;
		}
		if (!new_text_line.empty())
		{
			RenderIntroLine(x_left, x_right, ypos, new_text_line, num_spaces, final_size);
		}
		if (final_size == 0)
		{
			break;
		}
		ypos += LINE_HEIGHT;
		if (curParagraph == 0)
		{
			if (ypos > curData.paragraph[0].y_extent)
			{
				curParagraph++;
			}
		}
		else if (curParagraph == 1)
		{
			if (ypos > curData.paragraph[1].y_extent)
			{
				curParagraph++;
			}
		}
	}


	m_sdl_helper->SetRenderTarget(nullptr);
}
