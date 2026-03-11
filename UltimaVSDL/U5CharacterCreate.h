#pragma once

#include "GameObject.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <string>
#include <vector>
#include <functional>
#include "U5Enums.h"
#include <queue>

class U5CharacterCreate : public GameObject
{
public:
	explicit U5CharacterCreate(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5CharacterCreate();

	void Render() override;
	void ProcessEvents() override;
	virtual void LoadData() override;
	void SetSDLData() override;
	void SetCutScreenInfo(U5Modes old_mode, std::function<void(void)> callback);
private:
	void RenderCharacterCreate();
	void RenderStoryTexture();
	void RenderQuestionTexture(int question_num);
	void RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size, int SPACE_LEN = 6);
	int GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string& str_out, int& num_spaces, int& final_size, int SPACE_LEN = 6);
	void IncrementStory();

	int m_window_width;
	int m_window_height;
	std::function<void(void)> m_callback;
	U5Modes m_oldMode;
	std::vector<U5StoryScreen> m_story;
	int m_curStoryboard;
	int m_number_screens;
	std::vector< QuestionVirtue> m_virtue_list;
	std::queue<int> m_question_list;
	int m_current_question;

	const int NUM_CREATE_SCREENS = 9;
};

