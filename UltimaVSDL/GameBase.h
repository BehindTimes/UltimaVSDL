#pragma once

#include "GameObject.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include <utility>

class GameBase : public GameObject
{
public:
	explicit GameBase(SDL3Helper* sdl_helper, UltimaVResource* u5_resources);
	virtual ~GameBase();
	std::pair<float, float> m_DisplayOffset;
	void SetPos(int x, int y);
	void GetPos(int &x, int &y);
protected:
	virtual void DrawBorder();
	int m_xpos;
	int m_ypos;
	bool m_smoothscroll;
};

