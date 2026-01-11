#pragma once

#include "GameObject.h"

class Intro : public GameObject
{
public:
	explicit Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~Intro();

	void Render();
private:
	void RenderFadeIn();
};

