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

	std::vector < std::vector<unsigned char> > m_currentMap;
	GameLocation m_location;
	std::unique_ptr<U5Console> m_console;
	GameLocation m_old_location;
	std::pair<int, int> m_old_position;

private:
	int m_window_width;
	int m_window_height;

	
	std::unique_ptr<U5World> m_world;
	std::unique_ptr<U5Dungeon> m_dungeon;
	GameBase* m_curLocation;
	
};

