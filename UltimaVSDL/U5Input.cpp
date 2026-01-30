#include "U5Input.h"
#include <SDL3/SDL_events.h>
#include "SDL3Helper.h"
#include <vector>
#include <SDL3/SDL_keycode.h>
#include <algorithm>
#include <SDL3/SDL_stdinc.h>
#include <iostream>
#include <iterator>

U5Input::U5Input(SDL3Helper* sdl_helper) :
	m_sdl_helper(sdl_helper),
	m_curTick(0),
	m_prevTick(0),
	m_elapsedTick(0),
	m_InputType(InputType::ANY_KEY),
	m_key_delay(250),
	m_enabled(true),
	m_allow(true),
	m_curElapsedTime(0),
	m_anyKeyHit(false),
	m_allKeysMustBeUp(false),
	m_allowDiagonals(false)
{
	m_allowDiagonals = true;
	m_curTick = m_sdl_helper->GetCurrentTick();
}

U5Input::~U5Input()
{
}

void U5Input::SetInputType(InputType inputType)
{
	m_InputType = inputType;
}

bool U5Input::isAnyKeyHit() const
{
	if (m_allKeysMustBeUp)
	{
		return false;
	}
	return m_anyKeyHit;
}

void U5Input::StartInput()
{
	m_anyKeyHit = false;
	m_prevTick = m_curTick;
	m_curTick = m_sdl_helper->GetCurrentTick();
	m_elapsedTick = m_curTick - m_prevTick;
	if (!m_allow)
	{
		m_curElapsedTime += m_elapsedTick;
	}
	if (m_allKeysMustBeUp)
	{
		if (m_curKeyCodes.empty())
		{
			m_allKeysMustBeUp = false;
		}
	}
}

void U5Input::FinishInput()
{
	if (!m_allow && m_curElapsedTime > m_key_delay)
	{
		m_allow = true;
	}

	if (m_allow && m_curKeyCodes.size() > 0)
	{
		// Process keys here
		m_anyKeyHit = true;
		m_allow = false;
		m_curElapsedTime = 0;
	}
}

void U5Input::ProcessKeyDown(SDL_KeyboardEvent event)
{
	if (std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), event.key) == m_curKeyCodes.end())
	{
		m_curKeyCodes.push_back(event.key);
	}
}

void U5Input::ProcessKeyUp(SDL_KeyboardEvent event)
{
	auto it = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), event.key);
	if (it != m_curKeyCodes.end())
	{
		m_curKeyCodes.erase(it);
	}
}

SDL_Keycode U5Input::GetKeyCode() const
{
	std::vector<SDL_Keycode>::const_iterator kc1;
	std::vector<SDL_Keycode>::const_iterator kc2;
	size_t distance1;
	size_t distance2;
	if (m_allKeysMustBeUp)
	{
		return 0;
	}

	if (m_allowDiagonals)
	{
		if (m_anyKeyHit && m_curKeyCodes.size() > 0)
		{
			switch (m_curKeyCodes[0])
			{
			case SDLK_UP:
				kc1 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_LEFT);
				kc2 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_RIGHT);
				if (kc1 == m_curKeyCodes.end() && kc2 == m_curKeyCodes.end())
				{
					return m_curKeyCodes[0];
				}
				else if (kc1 != m_curKeyCodes.end())
				{
					return SDLK_KP_7;
				}
				else if (kc2 != m_curKeyCodes.end())
				{
					return SDLK_KP_9;
				}
				else
				{
					distance1 = std::distance(m_curKeyCodes.begin(), kc1);
					distance2 = std::distance(m_curKeyCodes.begin(), kc2);
					return kc1 < kc2 ? SDLK_KP_7 : SDLK_KP_9;
				}
				break;
			case SDLK_DOWN:
				kc1 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_LEFT);
				kc2 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_RIGHT);
				if (kc1 == m_curKeyCodes.end() && kc2 == m_curKeyCodes.end())
				{
					return m_curKeyCodes[0];
				}
				else if (kc1 != m_curKeyCodes.end())
				{
					return SDLK_KP_1;
				}
				else if (kc2 != m_curKeyCodes.end())
				{
					return SDLK_KP_3;
				}
				else
				{
					distance1 = std::distance(m_curKeyCodes.begin(), kc1);
					distance2 = std::distance(m_curKeyCodes.begin(), kc2);
					return kc1 < kc2 ? SDLK_KP_1 : SDLK_KP_3;
				}
				break;
			case SDLK_LEFT:
				kc1 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_UP);
				kc2 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_DOWN);
				if (kc1 == m_curKeyCodes.end() && kc2 == m_curKeyCodes.end())
				{
					return m_curKeyCodes[0];
				}
				else if (kc1 != m_curKeyCodes.end())
				{
					return SDLK_KP_7;
				}
				else if (kc2 != m_curKeyCodes.end())
				{
					return SDLK_KP_1;
				}
				else
				{
					distance1 = std::distance(m_curKeyCodes.begin(), kc1);
					distance2 = std::distance(m_curKeyCodes.begin(), kc2);
					return kc1 < kc2 ? SDLK_KP_7 : SDLK_KP_1;
				}
				break;
			case SDLK_RIGHT:
				kc1 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_UP);
				kc2 = std::find(m_curKeyCodes.begin(), m_curKeyCodes.end(), SDLK_DOWN);
				if (kc1 == m_curKeyCodes.end() && kc2 == m_curKeyCodes.end())
				{
					return m_curKeyCodes[0];
				}
				else if (kc1 != m_curKeyCodes.end())
				{
					return SDLK_KP_9;
				}
				else if (kc2 != m_curKeyCodes.end())
				{
					return SDLK_KP_3;
				}
				else
				{
					distance1 = std::distance(m_curKeyCodes.begin(), kc1);
					distance2 = std::distance(m_curKeyCodes.begin(), kc2);
					return kc1 < kc2 ? SDLK_KP_9 : SDLK_KP_3;
				}
				break;
			default:
				return m_curKeyCodes[0];
			}
		}
	}
	else
	{
		return m_curKeyCodes[0];
	}
	
	return 0;
}

void U5Input::SetKeyDelay(Uint64 delay)
{
	m_key_delay = delay;
}

void U5Input::EnableInput(bool enabled)
{
	m_enabled = enabled;
}

void U5Input::SetRequireAllKeysUp()
{
	m_allKeysMustBeUp = true;
}
