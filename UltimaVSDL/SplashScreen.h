#pragma once
#include "GameObject.h"
#include <SDL3/SDL_stdinc.h>
#include "SDL3Helper.h"
#include "UltimaVResource.h"

enum class SplashScreenMode
{
	LOGO,
	SIGNATURE
};

class SplashScreen : public GameObject
{
public:
	explicit SplashScreen(SDL3Helper *sdl_helper, UltimaVResource *u5_resources);
	~SplashScreen();

	void Render();
	virtual void LoadData() override;
	void SetSDLData() override;
protected:
	int m_CurrentLogo;

	Uint64 m_curDelay;
	SplashScreenMode m_curMode;
	SplashScreenMode m_oldMode;
private:
	void RenderLogo();
	void RenderSignature();
	int LoadSignaturePath(float percent);

	const Uint64 LOGO_ROTATE_DELAY = 750;
	const Uint64 LOGO_DELAY = 2000;
	const Uint64 A_DELAY = 1000;
	const Uint64 FULL_SIG_DISPLAY = 4000;
	const Uint64 FULL_SIG_DISPLAY_DONE = 6000;
};

