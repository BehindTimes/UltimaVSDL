#include "U5Input.h"
#include <SDL3/SDL_events.h>
#include "SDL3Helper.h"
#include <vector>
#include <SDL3/SDL_keycode.h>
#include <algorithm>
#include <SDL3/SDL_stdinc.h>

U5Input::U5Input(SDL3Helper* sdl_helper) :
	m_sdl_helper(sdl_helper),
	m_anyKeyHit(false),
	m_anyKeyHitDelay(0),
	m_curTick(0),
	m_prevTick(0),
	m_keyDelay(0),
	m_allowProcess(true),
	m_elapsedTick(0),
	m_allowQueueKeyHit(true),
	m_InputType(InputType::ANY_KEY),
	m_key_code(0),
	KEY_DELAY(100)
{
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
	return m_anyKeyHit && m_allowQueueKeyHit;
}

void U5Input::StartInput()
{
	m_prevTick = m_curTick;
	m_curTick = m_sdl_helper->GetCurrentTick();
	m_elapsedTick = m_curTick - m_prevTick;
	m_anyKeyHit = false;
}

void U5Input::FinishInput()
{
	if (!m_allowProcess)
	{
		m_keyDelay += m_elapsedTick;
		if (m_keyDelay > KEY_DELAY)
		{
			m_keyDelay %= KEY_DELAY;
			m_allowProcess = true;
			m_allowQueueKeyHit = true;
		}
		else
		{
			m_allowQueueKeyHit = false;
		}
	}
	else
	{
		// TO DO: This will depend on the keyboard input type
		// That is, we may want to ignore certain keys, so who cares if they press another button
		if (m_anyKeyHit)
		{
			m_allowProcess = false;
			m_allowQueueKeyHit = true;
		}
	}
}

void U5Input::ProcessKeyDown(SDL_KeyboardEvent event)
{
	//std::vector<SDL_Keycode> UDKR_vec = { SDLK_RETURN, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT };
	std::vector<SDL_Keycode> UDR_vec = { SDLK_RETURN, SDLK_UP, SDLK_DOWN };

	if (m_allowProcess == true)
	{
		// Start the delay timer
		m_keyDelay = 0;
		// Get the pressed key now
		switch (m_InputType)
		{
		case InputType::UP_DOWN_ENTER:
			if (std::find(UDR_vec.begin(), UDR_vec.end(), event.key) != UDR_vec.end())
			{
				m_anyKeyHit = true;
				m_key_code = event.key;
			}
			break;
		default:
			m_anyKeyHit = true;
			m_key_code = event.key;
			break;
		}
	}
}

void U5Input::ProcessKeyUp(SDL_KeyboardEvent event)
{
}

SDL_Keycode U5Input::GetKeyCode() const
{
	return m_key_code;
}

void U5Input::SetKeyDelay(Uint64 delay)
{
	KEY_DELAY = delay;
}
