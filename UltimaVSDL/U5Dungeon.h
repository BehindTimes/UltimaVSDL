#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <functional>

enum class DungeonType
{
	CAVE,
	CAVE1,
	DUNGEON
};

class U5Game;

class U5Dungeon : public GameBase
{
public:
	explicit U5Dungeon(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Dungeon();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	void LoadDungeonType(DungeonType type);
	void SetParent(U5Game* parent);
private:
	void DrawRoom();
	void DrawBorder();
	void ProcessAnyKeyHit();

	void TurnLeft();
	void TurnRight();

	std::function<void()> m_process_key;

	DungeonType m_dungeon_type;
	U5Game* m_parent;
};

