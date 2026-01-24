#pragma once
#include "SDL3Helper.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

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
	void SetInputType(InputType inputType);
	SDL_Keycode GetKeyCode() const;
private:
	SDL3Helper* m_sdl_helper;
	bool m_anyKeyHit;
	Uint64 m_anyKeyHitDelay;
	Uint64 m_curTick;
	Uint64 m_prevTick;
	Uint64 m_elapsedTick;
	Uint64 m_keyDelay;
	bool m_allowProcess;
	bool m_allowQueueKeyHit;

	const Uint64 KEY_DELAY = 100;
	InputType m_InputType;
	SDL_Keycode m_key_code;
};

