#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <utility>
#include <SDL3/SDL_stdinc.h>
#include <vector>
#include "U5Enums.h"
#include <functional>
#include <string>

struct PositionData
{
	PositionData() :
		elapsed_time(0)
	{
	}
	std::pair<int, int> old_position;
	std::pair<int, int> new_position;
	Uint64 elapsed_time;
	const Uint64 TURN_TIME = 120;
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
	std::vector<PositionData> m_vec_pos;

	GameLocation m_location_type;
private:
	void DrawNPCs();
	void DrawAvatar();
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
	void ProcessKlimb();
	void ProcessLook();
	void ProcessTalk();
	void ProcessYell();

	void ProcessAnyKeyHit();
	int ProcessYesNo();
	int ProcessDirection();
	int ProcessLetterImmediate();

	void ProcessLeaveTown();
	void HandleLeaveTown();
	void HandleKlimb();
	void HandleLook();
	void HandleTalk();
	void HandleYell();

	void StartTalk();
	void StartYell();

	bool DoYell();
	void DoTalk();

	void HandleNorth();
	void HandleSouth();
	void HandleEast();
	void HandleWest();
	void HandleNortheast();
	void HandleNorthwest();
	void HandleSoutheast();
	void HandleSouthwest();

	void PreMove();
	void PostMove();

	void PrintSign(int x, int y, int z);

	void ProcessTalkInput();
	void HandleTalkInput();

	std::function<void()> m_process_key;

	U5Game* m_parent;
	bool m_allowMove;
	std::string m_displayWord;
	int m_currentDialog;
};

