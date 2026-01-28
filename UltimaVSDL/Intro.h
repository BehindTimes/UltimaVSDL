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
#include "AnimationTexture.h"
#include <map>

enum class IntroMode : uint_fast16_t
{
	FADE_LOGO,
	FADE_FLAME_1,
	FADE_FLAME_2,
	MENU,
	ACKNOWLEDGEMENTS,
	DEMO
};

enum class MenuChoices :uint_fast16_t
{
	JOURNEY_ONWARD = 0,
	CREATE_NEW_CHARACTER,
	TRANSFER_CHARACTER,
	INTRODUCTION,
	ACKNOWLEDGEMENTS,
	RETURN_TO_VIEW
};

enum class AcknowlegementType
{
	SCROLL_UP,
	OPEN,
	CLOSE,
	SCROLL_DOWN,
	WAIT
};

struct DemoCharacter
{
	DemoCharacter() :
		tile(0),
		x(0),
		y(0),
		isFade(false),
		isVisible(true)
	{
	}
	DemoCharacter(int _tile, int _x, int _y) :
		tile(_tile),
		x(_x),
		y(_y),
		isFade(false),
		isVisible(true)
	{
	}
	int tile;
	int x;
	int y;
	bool isFade;
	bool isVisible;
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
	void RenderDemo();
	void RenderCursor();
	void RenderAcknowledgements();
	
	void RenderIntroBox();
	void CreateIntroBox();
	void CreateMenu();
	void CreateDemo();
	void ProcessDemoScript();
	
	void StoryOverCallback();

	Uint64 m_curDelayFlame;

	Uint64 m_curWodFade;
	int m_curFlame;

	const Uint64 FLAME_DELAY = 100;
	const Uint64 LOGO_FADE_DELAY = 3000;
	const Uint64 FLAME_FADE_DELAY = 3000;
	const Uint64 WOD_FADE_DELAY = 3000;
	const Uint64 ACKNOWLEDGEMENT_SCROLL_DELAY = 500;
	const Uint64 ACKNOWLEDGEMENT_OPEN_DELAY = 1000;

	const Uint64 SCRIPT_TICK = 200;
	const Uint64 SCREEN_OPEN_DELAY = SCRIPT_TICK * 12;
	const Uint64 TILE_FADE_DELAY = SCRIPT_TICK * 3;
	
	IntroMode m_curMode;
	int m_window_width;
	int m_window_height;
	int m_curMenuIndex;
	int m_demoInstructionNum;

	int m_demoBackground;
	bool m_smoothDemoOpen;

	std::unique_ptr<FadeObject> m_LogoFade;
	std::unique_ptr<FadeObject> m_FlameFade;
	std::unique_ptr<FadeObject> m_WoDFade;
	std::unique_ptr<FadeObject> m_DemoTileFade;
	float m_curAcknowledgementYPos;
	float m_curAcknowledgementXPos;
	Uint64 m_curAcknowledgementYDelay;
	Uint64 m_curAcknowledgementXDelay;
	Uint64 m_curInstructionDelay;
	Uint64 m_curDelayTime;
	bool m_demo_screen_open;
	bool m_fadeIn;
	Uint64 m_curScreenOpenDelay;
	int m_DemoMap[DEMO_WIDTH][DEMO_HEIGHT];
	int m_DemoFadeTileNum;
	int m_numLoops;
	int m_loopPos;

	AcknowlegementType m_curAcknowledgement;
	std::map<int, DemoCharacter> m_map_demo_char;
};

