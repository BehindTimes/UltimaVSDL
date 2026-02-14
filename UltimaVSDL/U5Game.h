#pragma once
#include "GameObject.h"
#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include "U5World.h"
#include <memory>
#include "U5Dungeon.h"
#include "GameBase.h"
#include <vector>
#include "U5Enums.h"
#include "U5Console.h"
#include <utility>
#include <cstdint>
#include <map>

class U5Game : public GameObject
{
public:
	explicit U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Game();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	virtual void LoadData() override;
	void SetSDLData() override;
	void GetElapsedTime() override;
	void LoadMap(int map_num);
	void ChangeLevel(int map_level);

	std::vector < std::vector<unsigned char> > m_currentMap;
	std::vector < std::vector<std::pair<uint8_t, uint8_t>>> m_currentDungeonMap;
	NPC_Info* m_curNPCs;
	GameLocation m_location;
	std::unique_ptr<U5Console> m_console;
	GameLocation m_old_location;
	std::pair<int, int> m_old_position;
	int m_map_level;
	int m_cur_level;
	std::vector< U5SignData> m_sign_data;
	std::map<int, U5Dialog> m_talk_data;

private:
	void LoadNPCData();

	int m_window_width;
	int m_window_height;
	int m_map_type;
	
	std::unique_ptr<U5World> m_world;
	std::unique_ptr<U5Dungeon> m_dungeon;
	GameBase* m_curLocation;
	
};

