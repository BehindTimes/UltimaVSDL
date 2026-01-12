#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"

GameObject::GameObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources) :
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources),
	m_clearScreen(false)
{
}

GameObject::~GameObject()
{
}

void GameObject::LoadData()
{

}

void GameObject::SetSDLData()
{
	m_curTick = m_sdl_helper->GetCurrentTick();
	m_prevTick = m_curTick;
	m_tickElapse = 0;
}

void GameObject::GetElapsedTime()
{
	m_prevTick = m_curTick;
	m_curTick = m_sdl_helper->GetCurrentTick();
	m_tickElapse = m_curTick - m_prevTick;
}

bool GameObject::ChangeMode(U5Modes& newMode) const
{
	bool retval = false;
	if (m_newMode != U5Modes::None)
	{
		newMode = m_newMode;
		retval = true;
	}
	return retval;
}
