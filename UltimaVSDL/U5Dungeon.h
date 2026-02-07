#pragma once

#include "GameBase.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <functional>
#include <cstdint>
#include <utility>

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
	bool DrawFirstLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	bool DrawSecondLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	bool DrawThirdLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	bool DrawFourthLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	void DrawBorder();
	void ProcessAnyKeyHit();

	void TurnLeft();
	void TurnRight();

	std::function<void()> m_process_key;

	DungeonType m_dungeon_type;
	U5Game* m_parent;
};

