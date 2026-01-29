#pragma once
#include "GameObject.h"
#include "UltimaVResource.h"
#include "SDL3Helper.h"

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

	void DrawBorder();
	GameLocation m_location;
};

