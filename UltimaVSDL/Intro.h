#pragma once

#include "GameObject.h"
#include <cstdint>
#include <vector>
#include <SDL3/SDL_stdinc.h>
#include "SDL3Helper.h"
#include "UltimaVResource.h"

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
	void RenderIntroBox();
	void CreateIntroBox();
	void IncrementStory();

	Uint64 m_curDelayFlame;
	Uint64 m_curLogoFade;
	Uint64 m_curFlame1Fade;
	Uint64 m_curWodFade;
	int m_curFlame;

	const Uint64 FLAME_DELAY = 100;
	const Uint64 LOGO_FADE_DELAY = 3000;
	const Uint64 FLAME_FADE_1_DELAY = 3000;
	const Uint64 WOD_FADE_DELAY = 3000;
	IntroMode m_curMode;
	int m_window_width;
	int m_window_height;
	std::vector<uint8_t> m_logo_fade_locations;
	std::vector<uint8_t> m_flame1_fade_locations;
	std::vector<uint8_t> m_wod_fade_locations;
	int m_logo_fade_count;
	int m_flame1_fade_count;
	int m_wod_fade_count;
	int m_cur_logo_count;
	int m_cur_flame1_fade_count;
	int m_cur_wod_fade_count;

	uint32_t m_num_pixels;
	uint32_t m_num_flame1_pixels;
	uint32_t m_num_wod_pixels;
	uint32_t m_curStoryboard;
};

