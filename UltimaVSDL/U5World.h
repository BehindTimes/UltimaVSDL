#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"

class U5World : public GameBase
{
public:
	explicit U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5World();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	void DrawBorder() override;
};

