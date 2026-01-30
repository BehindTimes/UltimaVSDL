#pragma once
#include "GameObject.h"
#include "UltimaVResource.h"
#include "SDL3Helper.h"
#include "U5World.h"
#include <memory>
#include "U5Dungeon.h"
#include "GameBase.h"

enum class GameLocation
{
	Dungeon,
	World
};

class U5Game : public GameObject
{
public:
	explicit U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	~U5Game();

	virtual void Render() override;
	virtual void ProcessEvents() override;
	virtual void LoadData() override;
	void SetSDLData() override;
private:
	int m_window_width;
	int m_window_height;

	GameLocation m_location;
	std::unique_ptr<U5World> m_world;
	std::unique_ptr<U5Dungeon> m_dungeon;
	GameBase* m_curLocation;
};

