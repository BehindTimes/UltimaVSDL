#include <memory>
#include <cstdlib>
#include <SDL3/SDL_main.h>

#include "SDL3Helper.h"

#include "Intro.h"
#include "SplashScreen.h"
#include "U5Utils.h"
#include "CutScene.h"
#include <SDL3/SDL_main_impl.h>
#include "GameObject.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include "U5Input.h"
#include "U5Game.h"
#include "GameOptions.h"
#include "CharacterData.h"
#include "U5CharacterCreate.h"

static std::unique_ptr<SDL3Helper> sdl_helper;
static std::unique_ptr<UltimaVResource> u5_resources;
static std::unique_ptr<SplashScreen> splash_screen;
static std::unique_ptr<Intro> intro_screen;

std::unique_ptr<CutScene> cutscene_screen;
std::unique_ptr<U5Utils> m_utilities;
std::unique_ptr<U5Input> m_input;
std::unique_ptr<U5Game> m_game;
std::unique_ptr<GameOptions> m_options;
std::unique_ptr<CharacterData> m_charData;
std::unique_ptr<U5CharacterCreate> m_characterCreate;

static void MainLoop()
{
	bool quit = false;
	GameObject* curObject = nullptr;

	m_input = std::make_unique<U5Input>(sdl_helper.get());
	m_utilities = std::make_unique<U5Utils>();

	u5_resources = std::make_unique<UltimaVResource>();
	if (0 != u5_resources->LoadResources())
	{
		return;
	}
	sdl_helper->LoadImageData(u5_resources.get());

	sdl_helper->UpdateTicks();

	splash_screen = std::make_unique<SplashScreen>(sdl_helper.get(), u5_resources.get());
	splash_screen->LoadData();

	intro_screen = std::make_unique<Intro>(sdl_helper.get(), u5_resources.get());
	intro_screen->LoadData();

	cutscene_screen = std::make_unique<CutScene>(sdl_helper.get(), u5_resources.get());
	cutscene_screen->LoadData();

	m_game = std::make_unique<U5Game>(sdl_helper.get(), u5_resources.get());
	m_game->LoadData();

	m_characterCreate = std::make_unique<U5CharacterCreate>(sdl_helper.get(), u5_resources.get());
	m_characterCreate->LoadData();

	m_charData = std::make_unique<CharacterData>();

	//curObject = m_game.get();
	//curObject = splash_screen.get();
	curObject = intro_screen.get();
	curObject->LoadData();
	
	while (1)
	{
		if (!curObject)
		{
			break;
		}
		sdl_helper->Poll();
		if (sdl_helper->m_quit)
		{
			quit = true;
		}
		curObject->Render();

		if (quit)
		{
			break;
		}
		curObject->ProcessEvents();
		sdl_helper->UpdateTicks();
		curObject->GetElapsedTime();

		U5Modes new_mode;
		if (curObject->ChangeMode(new_mode))
		{
			switch (new_mode)
			{
			case U5Modes::Menu:
				intro_screen->LoadData();
				curObject = intro_screen.get();
				break;
			case U5Modes::MenuSkip:
				intro_screen->LoadData();
				intro_screen->GoToSelection();
				curObject = intro_screen.get();
				break;
			case U5Modes::Cutscene:
				cutscene_screen->LoadData();
				curObject = cutscene_screen.get();
				break;
			case U5Modes::Game:
				m_game->LoadData();
				curObject = m_game.get();
				break;
			case U5Modes::CreateCharacter:
				m_characterCreate->LoadData();
				curObject = m_characterCreate.get();
				break;
			default:
				curObject = nullptr;
				break;
			}
			new_mode = U5Modes::None;
		}
	}
}

int main([[maybe_unused]]int argc, [[maybe_unused]] char** argv)
{
	bool valid;
	m_options = std::make_unique<GameOptions>();
	if (m_options->Initialize() != 0)
	{
		return EXIT_FAILURE;
	}

	sdl_helper = std::make_unique<SDL3Helper>();
	valid = sdl_helper->Intialize();

	MainLoop();

	if (!valid)
	{
		EXIT_FAILURE;
	}
	sdl_helper->Cleanup();
	return EXIT_SUCCESS;
}