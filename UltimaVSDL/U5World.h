#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <utility>
#include <SDL3/SDL_stdinc.h>
#include <vector>
#include "U5Enums.h"

struct PositionData
{
	PositionData() :
		elapsed_time(0)
	{
	}
	std::pair<int, int> old_position;
	std::pair<int, int> new_position;
	Uint64 elapsed_time;
	const Uint64 TURN_TIME = 125;
};

class U5Game;

class U5World : public GameBase
{
public:
	explicit U5World(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5World();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	void DrawBorder() override;
	void SetParent(U5Game* parent);
	std::vector<PositionData> vec_pos;
private:
	void ProcessScroll();
	int checkValidLocation(const PositionData& pos_info);
	void ProcessEnter();
	void ProcessNorth();
	void ProcessSouth();
	void ProcessEast();
	void ProcessWest();
	void ProcessNorthEast();
	void ProcessNorthWest();
	void ProcessSouthEast();
	void ProcessSouthWest();

	GameLocation m_location_type;
	U5Game* m_parent;
};

