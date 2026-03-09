#include "U5CharacterCreate.h"
#include <functional>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include <string>
#include "U5Input.h"
#include <memory>
#include <iostream>
#include <vector>

extern std::unique_ptr<U5Input> m_input;

U5CharacterCreate::U5CharacterCreate(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_window_width(0),
	m_window_height(0)
{
}

U5CharacterCreate::~U5CharacterCreate()
{
}

void U5CharacterCreate::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();
	RenderCharacterCreate();
	m_sdl_helper->RenderPresent();
}

void U5CharacterCreate::LoadData()
{
	SetSDLData();
	//CreateStreamingTextures();
	//RenderStoryTexture();
}

void U5CharacterCreate::SetSDLData()
{
	GameObject::SetSDLData();
}

void U5CharacterCreate::ProcessEvents()
{
	/*if (m_input->isAnyKeyHit())
	{
		IncrementStory();
	}*/
}

void U5CharacterCreate::RenderCharacterCreate()
{
	U5StoryScreen& curData = m_story[1];

	auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number)];
	auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number)];

	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	auto& curImageData = curStoryImages[10];
	auto& curImageTexture = curStoryTextures[10];

	// In the original, this is rendered after images rendered in action 1, but render this before to avoid cutting off the text
	m_sdl_helper->RenderTextureAt(curImageTexture, curData.picture_x * hMult, curData.picture_y * vMult, curImageData.width * hMult, curImageData.height * vMult);

	m_sdl_helper->RenderTextureAt(m_sdl_helper->m_FullScreenTexture, 0, 0, RENDER_WIDTH, RENDER_HEIGHT);
}

void U5CharacterCreate::RenderStoryTexture()
{
	U5StoryScreen story_screen;
	const int LINE_HEIGHT = 9;

	if (m_story.size() == 0)
	{
		return;
	}

	auto& curData = m_story[IV16_CREATE + 1];

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_FullScreenTexture);
	m_sdl_helper->ClearScreen();


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

void U5CharacterCreate::RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size)
{
	// We always end on a letter, so remove the letter spacing of the very last letter
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	const size_t DASH_POS = 13;
	const int SPACE_LEN = 5;
	const int TAB_LEN = 15;
	int total_size = (x_right - x_left) - 2;
	int space_size = 0;

	int remainder = 0;

	if (num_spaces > 0 && final_size > 0)
	{
		space_size = (total_size - (final_size + 1)) / num_spaces;
		remainder = (total_size - (final_size + 1)) % num_spaces;
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
			if (cur_left > 0)
			{
				cur_left--;
			}
			//cur_left += SPACE_LEN + space_size /*+ (remainder > 0 ? 1 : 0)*/;
			cur_left += SPACE_LEN + space_size;

			if (index > (num_spaces - remainder))
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
int U5CharacterCreate::GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string& str_out,
	int& num_spaces, int& final_size)
{
	const int SPACE_LEN = 5;
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

	bool ppp = false;
	//if (start_word == 198)
	if (start_word == 0)
	//if (start_word == 137)
	{
		int j = 9;
		ppp = true;
	}

	for (size_t index = start_word; index < letter_list.size(); index++)
	{
		unsigned char temp_letter = letter_list[index];
		int temp_len = 0;
		if (temp_letter == 0x7b)
		{
			temp_len = TAB_LEN/* - 1*/;
		}
		else if (temp_letter == 0x0A) // newline
		{
			final_size = -1;
			return static_cast<int>(index + 1);
		}
		else if (temp_letter == 0x20) // space
		{
			temp_len = SPACE_LEN/* - 1*/;
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
		if (ppp)
		{
			std::cout << ((char)temp_letter) << "\t" << (curlen + temp_len) << std::endl;
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
			if (curlen > 0)
			{
				curlen--;
			}
			curlen--;
			num_spaces++;
		}
		else if (temp_letter == 0x7b)
		{
			if (curlen > 0)
			{
				curlen--;
			}
			curlen--;
		}
	}
	return 0;
}

void U5CharacterCreate::SetCutScreenInfo(U5Modes old_mode, std::function<void(void)> callback)
{
	m_input->SetInputType(InputType::ANY_KEY);
	m_oldMode = old_mode;
	m_callback = callback;
	m_story = m_resources->m_data.question_text;
	RenderStoryTexture();
}
