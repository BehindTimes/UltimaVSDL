#include "CutScene.h"
#include <functional>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include <string>
#include "U5Input.h"
#include <cstdint>
#include <memory>
#include <vector>
#include "FadeObject.h"

extern std::unique_ptr<U5Input> m_input;

CutScene::CutScene(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_number_screens(0),
	m_story_id(0),
	m_curStoryboard(0),
	m_window_width(0),
	m_window_height(0),
	m_oldMode(U5Modes::None)
{
	m_clearScreen = true;

	m_CodexFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_CodexFade->SetSize(static_cast<size_t>(CODEX_WIDTH * CODEX_HEIGHT));
	m_CodexFade->SetDelay(CODEX_FADE_DELAY);
}

CutScene::~CutScene()
{
}

void CutScene::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();
	RenderStory();
	m_sdl_helper->RenderPresent();
}

void CutScene::LoadData()
{
	SetSDLData();
	CreateStreamingTextures();
	//RenderStoryTexture();
}

void CutScene::SetSDLData()
{
	GameObject::SetSDLData();
}

void CutScene::CreateStreamingTextures()
{
	m_CodexFade->Reset();
}

void CutScene::ProcessEvents()
{
	if (m_input->isAnyKeyHit())
	{
		IncrementStory();
	}
}

void CutScene::SetCutScreenInfo(U5Modes old_mode, std::vector<U5StoryScreen>& story, int story_id, std::function<void(void)> callback)
{
	m_input->SetInputType(InputType::ANY_KEY);
	m_oldMode = old_mode;
	m_story = story;
	m_number_screens = static_cast<int>(story.size());
	m_story_id = story_id;
	m_callback = callback;
	m_CodexFade->Reset();
	m_sdl_helper->TurnOnAllPixels(m_sdl_helper->m_CodexFadeTexture, false);
	RenderStoryTexture();
}

void CutScene::RenderStory()
{
	const int NUM_STORY_DATA = 6;
	if (m_curStoryboard >= m_number_screens || m_story.size() != m_number_screens)
	{
		m_newMode = m_oldMode;
		if (m_callback)
		{
			m_callback();
		}
		return;
	}
	U5StoryScreen& curData = m_story[m_curStoryboard];
	if (curData.story_number >= NUM_STORY_DATA || curData.story_number >= m_sdl_helper->m_Image16FileTextures.size())
	{
		return;
	}
	auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number + m_story_id)];
	auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number + m_story_id)];

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

	// In the original, this is rendered after images rendered in action 1, but render this before to avoid cutting off the text
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
			if (m_story_id == 1) // The ending
			{
				title_x_pos[0] = 224;
				title_y_pos[0] = 0;
				title_x_pos[1] = 152;
				title_y_pos[1] = 29;
				title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
				title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][4];
				title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
				title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][4];
			}
			else // The intro screen
			{
				title_x_pos[0] = 224;
				title_y_pos[0] = 30;
				title_x_pos[1] = 168;
				title_y_pos[1] = 59;
				title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
				title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][1];
				title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
				title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][1];
			}
			break;
		case 1:
			if (m_story_id == 1) // The ending
			{
				title_x_pos[0] = 176;
				title_y_pos[0] = 0;
				title_x_pos[1] = 224;
				title_y_pos[1] = 0;
				title_texture[0] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][0];
				title_texture[1] = m_sdl_helper->m_Image16FileTextures[IV16_TEXT][5];
				title_data[0] = m_resources->m_Image16FileData[IV16_TEXT][0];
				title_data[1] = m_resources->m_Image16FileData[IV16_TEXT][5];
			}
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

void CutScene::RenderStoryTexture()
{
	const int LINE_HEIGHT = 9;
	const size_t ANKH_LOCATION = 2;

	if (m_story.size() == 0)
	{
		return;
	}

	auto& curData = m_story[m_curStoryboard];

	// This is a very specific case, where it will copy 45x34 of the third texture to fade in
	// Now, this is only done in one place in the game, but let's stay compliant to the original engine
	if (curData.action == 2)
	{
		if (m_resources->m_Image16FileData[static_cast<size_t>(m_story_id + curData.story_number)].size() > (ANKH_LOCATION) &&
			m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(m_story_id + curData.story_number)].size() > (ANKH_LOCATION))
		{
			auto& curTexture = m_resources->m_Image16FileData[static_cast<size_t>(m_story_id + curData.story_number)][ANKH_LOCATION];
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
		m_sdl_helper->SetRenderTarget(nullptr);
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

void CutScene::RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size, int SPACE_LEN)
{
	// We always end on a letter, so remove the letter spacing of the very last letter
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	const size_t DASH_POS = 13;
	//const int SPACE_LEN = 5;
	const int TAB_LEN = 15;
	int total_size = (x_right - x_left);
	int space_size = 0;

	int remainder = 0;

	if (num_spaces > 0 && final_size > 0)
	{
		space_size = (total_size - (final_size)) / num_spaces;
		remainder = (total_size - (final_size)) % num_spaces;
	}

	int cur_left = x_left;
	int cur_space = 0;

	for (size_t index = 0; index < str_line.size(); index++)
	{
		if (str_line[index] == 0x7b)
		{
			cur_left += TAB_LEN;
			if (index != 0)
			{
				cur_left--;
			}

		}
		else if (str_line[index] == ' ')
		{
			cur_space++;
			if (index != 0)
			{
				cur_left--;
			}

			cur_left += SPACE_LEN + space_size;

			if (cur_space > (num_spaces - remainder))
			{
				cur_left++;
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

// The game wants full words, and will space those words appropriately.
// That is unless an _ is found, at which point, if that still fits, it will cut off at that and use a -.
int CutScene::GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string& str_out,
	int& num_spaces, int& final_size, int SPACE_LEN)
{
	//const int SPACE_LEN = 5;
	auto& dash_letter = m_resources->m_ProportionalFontData[13];
	int dash_len = dash_letter.real_width;
	const int TAB_LEN = 15;
	int max_len = (right - left);
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
			temp_len = TAB_LEN;
		}
		else if (temp_letter == 0x0A) // newline
		{
			final_size = -1;
			return static_cast<int>(index + 1);
		}
		else if (temp_letter == 0x20) // space
		{
			temp_len = SPACE_LEN;
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
						curlen -= (SPACE_LEN - 1);
						final_size = curlen;
						num_spaces--;
						temp_index++;
						return temp_index;
					}
					else if (back_char == '_')
					{
						if (curlen + (dash_len) < max_len)
						{
							str_out += "_";
							curlen += dash_len;
							final_size = curlen + 1;
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

		int orig_curlin = curlen;
		curlen += (temp_len + 1);
		str_out += temp_letter;
		if (temp_letter == ' ')
		{
			if (orig_curlin > 0)
			{
				curlen--;
			}
			curlen--;
			num_spaces++;
		}
		else if (temp_letter == 0x7b)
		{
			if (orig_curlin > 0)
			{
				curlen--;
			}
			curlen--;
		}
	}
	return 0;
}

void CutScene::IncrementStory()
{
	m_curStoryboard++;
	if (m_curStoryboard >= m_number_screens)
	{
		m_curStoryboard = 0;
		m_newMode = m_oldMode;
		if (m_callback)
		{
			m_callback();
		}
		return;
	}
	RenderStoryTexture();
}
