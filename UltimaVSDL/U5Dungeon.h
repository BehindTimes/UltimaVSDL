#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"

class U5Dungeon : public GameBase
{
public:
	explicit U5Dungeon(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Dungeon();

	virtual void Render() override;
	virtual void ProcessEvents() override;
};

