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
	void CalcCurMatrix();
	void SetPos(int x, int y) override;
private:
	void DrawRoom();
	bool DrawFirstLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right, bool hasSign);
	bool DrawSecondLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right, bool& hasSign);
	bool DrawThirdLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	bool DrawFourthLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right);
	void DrawBorder();
	void ProcessAnyKeyHit();
	int ProcessUpDown();

	void HandleKlimb();

	int checkValidLocation(const std::pair<int, int>& pos_info);
	void TurnLeft();
	void TurnRight();
	void TurnAround();
	void GoForward();
	void GoBackward();
	void Klimb();

	std::function<void()> m_process_key;

	DungeonType m_dungeon_type;
	U5Game* m_parent;

	std::pair<uint8_t, uint8_t> m_curMatrix[3][4];
};

