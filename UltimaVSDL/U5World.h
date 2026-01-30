#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <utility>
#include <SDL3/SDL_stdinc.h>
#include <vector>

struct PositionQueue
{
	PositionQueue() :
		elapsed_time(0)
	{
	}
	std::pair<int, int> old_position;
	std::pair<int, int> new_position;
	Uint64 elapsed_time;
	const Uint64 TURN_TIME = 250;
};

class U5World : public GameBase
{
public:
	explicit U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5World();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	void DrawBorder() override;
	std::vector<PositionQueue> vec_pos;
private:
	void ProcessScroll();
};

