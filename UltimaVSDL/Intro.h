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
	void RenderFlame();

	Uint64 m_curDelayFlame;
	int m_curFlame;

	const Uint64 FLAME_DELAY = 100;
};

