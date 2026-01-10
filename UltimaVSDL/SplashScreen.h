#pragma once
#include "GameObject.h"

class SplashScreen : public GameObject
{
public:
	explicit SplashScreen(SDL3Helper *sdl_helper, UltimaVResource *u5_resources);
	~SplashScreen();

	void Render();
	virtual void LoadData() override;
protected:
	
};

