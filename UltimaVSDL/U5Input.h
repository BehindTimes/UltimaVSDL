#pragma once
#include "SDL3Helper.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <vector>

enum class InputType
{
	UP_DOWN_ENTER,
	LEFT_RIGHT_ENTER,
	ANY_KEY
};

class U5Input
{
public:
	explicit U5Input(SDL3Helper* sdl_helper);
	~U5Input();

	bool isAnyKeyHit() const;
	void ProcessKeyDown(SDL_KeyboardEvent event);
	void ProcessKeyUp(SDL_KeyboardEvent event);
	void StartInput();
	void FinishInput();
	void EnableInput(bool enabled);
	void SetInputType(InputType inputType);
	SDL_Keycode GetKeyCode() const;
	void SetKeyDelay(Uint64 delay);
	void SetRequireAllKeysUp();
private:
	SDL3Helper* m_sdl_helper;

	Uint64 m_curTick;
	Uint64 m_prevTick;
	Uint64 m_elapsedTick;
	Uint64 m_curElapsedTime;
	
	bool m_enabled;
	Uint64 m_key_delay;
	InputType m_InputType;
	std::vector<SDL_Keycode> m_curKeyCodes;
	bool m_allow;
	bool m_anyKeyHit;
	bool m_allKeysMustBeUp;
};

