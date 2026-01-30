#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"

GameObject::GameObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources) :
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources),
	m_clearScreen(false),
	m_curTick(0),
	m_tickElapse(0),
	m_prevTick(0),
	m_newMode(U5Modes::None)
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

bool GameObject::ChangeMode(U5Modes& newMode)
{
	bool retval = false;
	if (m_newMode != U5Modes::None)
	{
		newMode = m_newMode;
		retval = true;
		m_newMode = U5Modes::None;
	}
	return retval;
}
