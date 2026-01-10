#pragma once
#include <iostream>

#include "SDL3Helper.h"
#include "UltimaVResource.h"

class GameObject
{
public:
	explicit GameObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources);
	virtual ~GameObject();

	virtual void Render() = 0;
	virtual void LoadData();
protected:
	SDL3Helper *m_sdl_helper;
	UltimaVResource *m_resources;
};

