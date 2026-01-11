#pragma once
#include <iostream>

#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include "U5Enums.h"

class GameObject
{
public:
	explicit GameObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources);
	virtual ~GameObject();

	virtual void Render() = 0;
	virtual void LoadData();
	void GetElapsedTime();
	bool ChangeMode(U5Modes &newMode);
	
protected:
	virtual void SetSDLData();

	SDL3Helper *m_sdl_helper;
	UltimaVResource *m_resources;
	Uint64 m_curTick;
	Uint64 m_prevTick;
	Uint64 m_tickElapse;
	U5Modes m_newMode;
	bool m_clearScreen;
};

