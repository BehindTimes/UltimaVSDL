#pragma once

#include "GameObject.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <SDL3/SDL_stdinc.h>
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include "FadeObject.h"

enum class IntroMode
{
	FADE_LOGO,
	FADE_FLAME_1,
	FADE_FLAME_2,
	SHOW_ALL,
	STORY
};

class Intro : public GameObject
{
public:
	explicit Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~Intro();

	void Render() override;
	void ProcessEvents() override;
	virtual void LoadData() override;
	void SetSDLData() override;
	void GoToSelection();
private:
	void RenderLogoFadeIn();
	void RenderFlameFadeIn1();
	void RenderFlameFadeWoD();
	void RenderFlame();
	void RenderLogo();
	void RenderWoD();
	void RenderStory();
	void RenderStoryTexture();
	void RenderIntroBox();
	void CreateIntroBox();
	void CreateStreamingTextures();
	void IncrementStory();
	int GetLine(int left, int right, size_t start_word, std::vector<unsigned char> letter_list, std::string &str_out, int& num_spaces, int& final_size);
	void RenderIntroLine(int x_left, int x_right, int y_pos, std::string str_line, int num_spaces, int final_size);

	Uint64 m_curDelayFlame;

	Uint64 m_curWodFade;
	int m_curFlame;

	const int CODEX_WIDTH = 36; // We only copy a portion of the andh texture
	const int CODEX_HEIGHT = 34;
	const Uint64 FLAME_DELAY = 100;
	const Uint64 LOGO_FADE_DELAY = 3000;
	const Uint64 FLAME_FADE_DELAY = 3000;
	const Uint64 WOD_FADE_DELAY = 3000;
	const Uint64 CODEX_FADE_DELAY = 1000;
	IntroMode m_curMode;
	int m_window_width;
	int m_window_height;

	uint32_t m_curStoryboard;

	std::unique_ptr<FadeObject> m_LogoFade;
	std::unique_ptr<FadeObject> m_FlameFade;
	std::unique_ptr<FadeObject> m_WoDFade;
	std::unique_ptr<FadeObject> m_CodexFade;
};

