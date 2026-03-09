#include "U5CharacterCreate.h"
#include <functional>
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
	m_window_height(0),
	m_oldMode(U5Modes::None),
	m_curStoryboard(0),
	m_number_screens(0)
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
	if (m_input->isAnyKeyHit())
	{
		IncrementStory();
	}
}

void U5CharacterCreate::RenderCharacterCreate()
{
	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	const int END_CREATE = 2;
	if (m_curStoryboard == 0)
	{
		U5StoryScreen& curData = m_story[0];

		auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number)];
		auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number)];

		auto& curImageData = curStoryImages[0];
		auto& curImageTexture = curStoryTextures[0];

		// In the original, this is rendered after images rendered in action 1, but render this before to avoid cutting off the text
		m_sdl_helper->RenderTextureAt(curImageTexture, curData.picture_x * hMult, curData.picture_y * vMult, curImageData.width * hMult, curImageData.height * vMult);
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_FullScreenTexture, 0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	}
	else if (m_curStoryboard == END_CREATE)
	{
		const int END_CREATE_PIC = 10;
		U5StoryScreen& curData = m_story[1];

		auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number)];
		auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number)];

		auto& curImageData = curStoryImages[END_CREATE_PIC];
		auto& curImageTexture = curStoryTextures[END_CREATE_PIC];

		// In the original, this is rendered after images rendered in action 1, but render this before to avoid cutting off the text
		m_sdl_helper->RenderTextureAt(curImageTexture, curData.picture_x * hMult, curData.picture_y * vMult, curImageData.width * hMult, curImageData.height * vMult);
		m_sdl_helper->RenderTextureAt(m_sdl_helper->m_FullScreenTexture, 0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	}
	else // Handle the character create stuff
	{
		U5StoryScreen& curData = m_story[0];
		auto& curStoryImages = m_resources->m_Image16FileData[static_cast<size_t>(curData.story_number)];
		auto& curStoryTextures = m_sdl_helper->m_Image16FileTextures[static_cast<size_t>(curData.story_number)];
		auto& curImageData = curStoryImages[1];
		auto& curImageTexture = curStoryTextures[1];

		// In the original, this is rendered after images rendered in action 1, but render this before to avoid cutting off the text
		m_sdl_helper->RenderTextureAt(curImageTexture, 0x10 * hMult, 0 * vMult, curImageData.width * hMult, curImageData.height * vMult);
		m_sdl_helper->RenderTextureAt(curImageTexture, 0xC8 * hMult, 0 * vMult, curImageData.width * hMult, curImageData.height * vMult);
		//m_sdl_helper->RenderTextureAt(m_sdl_helper->m_FullScreenTexture, 0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	}
}

void U5CharacterCreate::RenderStoryTexture()
{
	U5StoryScreen story_screen;
	const int LINE_HEIGHT = 9;

	if (m_story.size() == 0)
	{
		return;
	}

	U5StoryScreen curData;

	int SPACE_LEN = 6;

	if (m_curStoryboard == 0)
	{
		curData = m_story[0];
	}
	else
	{
		const int END_CREATE = 1;
		curData = m_story[END_CREATE];
		SPACE_LEN = 5;
	}

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_FullScreenTexture);
	m_sdl_helper->ClearScreen();


	int ypos = curData.text_y_pos;

	// Render the first sentence
	int x_left = curData.first_line_offset;
	int x_right = curData.paragraph[0].text_right_pos;
	std::string text_line;

	int num_spaces = 0;
	int final_size = 0;
	int new_text_pos = GetLine(x_left, x_right, 0, curData.text, text_line, num_spaces, final_size, SPACE_LEN);
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

	RenderIntroLine(x_left, x_right, ypos, text_line, num_spaces, final_size, SPACE_LEN);
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
		new_text_pos = GetLine(x_left, x_right, new_text_pos, curData.text, new_text_line, num_spaces, final_size, SPACE_LEN);
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
			RenderIntroLine(x_left, x_right, ypos, new_text_line, num_spaces, final_size, SPACE_LEN);
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

void U5CharacterCreate::RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size, int SPACE_LEN)
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
int U5CharacterCreate::GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string& str_out,
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

void U5CharacterCreate::SetCutScreenInfo(U5Modes old_mode, std::function<void(void)> callback)
{
	const int NUM_CREATE_SCREENS = 3;
	m_input->SetInputType(InputType::ANY_KEY);
	m_oldMode = old_mode;
	m_callback = callback;
	m_story = m_resources->m_data.question_text;
	m_curStoryboard = 0;
	m_number_screens = NUM_CREATE_SCREENS;
	RenderStoryTexture();
}

void U5CharacterCreate::IncrementStory()
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
	if (m_curStoryboard == 2)
	{
		m_input->SetInputType(InputType::ANY_KEY);
		RenderStoryTexture();
	}
	else
	{
		m_input->SetInputType(InputType::A_OR_B);
	}
}