#pragma once
#include <functional>

#include "GameObject.h"
#include <memory>
#include "FadeObject.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <SDL3/SDL_stdinc.h>
#include <string>

class CutScene : public GameObject
{
public:
	explicit CutScene(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~CutScene();

	void Render() override;
	void ProcessEvents() override;
	virtual void LoadData() override;
	void SetSDLData() override;

	void SetCutScreenInfo(U5Modes old_mode, std::vector<U5StoryScreen>& story, int story_id, std::function<void(void)> callback);

private:
	void RenderStory();
	void CreateStreamingTextures();
	void RenderStoryTexture();
	void RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size);
	int GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string& str_out, int& num_spaces, int& final_size);
	void IncrementStory();

	int m_number_screens;
	int m_story_id;
	int m_curStoryboard;
	int m_window_width;
	int m_window_height;

	const Uint64 CODEX_FADE_DELAY = 1000;
	const int CODEX_WIDTH = 36; // We only copy a portion of the andh texture
	const int CODEX_HEIGHT = 34;

	std::function<void(void)> m_callback;
	std::unique_ptr<FadeObject> m_CodexFade;
	U5Modes m_oldMode;
	std::vector<U5StoryScreen> m_story;
};

