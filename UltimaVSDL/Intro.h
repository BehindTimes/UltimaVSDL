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
	FADE_FLAME,
	SHOW_ALL
};

class Intro : public GameObject
{
public:
	explicit Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~Intro();

	void Render();
	virtual void LoadData() override;
	void SetSDLData() override;
private:
	void RenderFadeIn();
	void RenderFlame();
	void RenderLogo();

	Uint64 m_curDelayFlame;
	Uint64 m_curLogoFade;
	int m_curFlame;

	const Uint64 FLAME_DELAY = 100;
	const Uint64 LOGO_FADE_DELAY = 3000;
	IntroMode m_curMode;
	int m_window_width;
	int m_window_height;
	std::vector<uint8_t> m_logo_fade_locations;
	int m_logo_fade_count;
	int m_cur_logo_count;
};

