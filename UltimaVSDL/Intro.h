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
#include "U5Enums.h"

enum class IntroMode
{
	FADE_LOGO,
	FADE_FLAME_1,
	FADE_FLAME_2,
	MENU
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
	virtual bool ChangeMode(U5Modes& newMode) override;
private:
	void RenderLogoFadeIn();
	void RenderFlameFadeIn1();
	void RenderFlameFadeWoD();
	void RenderFlame();
	void RenderLogo();
	void RenderWoD();
	void RenderMenu();
	
	void RenderIntroBox();
	void CreateIntroBox();
	
	void StoryOverCallback();

	Uint64 m_curDelayFlame;

	Uint64 m_curWodFade;
	int m_curFlame;

	const Uint64 FLAME_DELAY = 100;
	const Uint64 LOGO_FADE_DELAY = 3000;
	const Uint64 FLAME_FADE_DELAY = 3000;
	const Uint64 WOD_FADE_DELAY = 3000;
	
	IntroMode m_curMode;
	int m_window_width;
	int m_window_height;

	std::unique_ptr<FadeObject> m_LogoFade;
	std::unique_ptr<FadeObject> m_FlameFade;
	std::unique_ptr<FadeObject> m_WoDFade;
};

