#include <iostream>
#include <cstdlib>
#include <SDL3/SDL_main.h>

#include "SDL3Helper.h"

#include "SplashScreen.h"

static std::unique_ptr<SDL3Helper> sdl_helper;
static std::unique_ptr<UltimaVResource> u5_resources;
static std::unique_ptr<SplashScreen> splash_screen;

void MainLoop()
{
	bool quit = false;
	u5_resources = std::make_unique<UltimaVResource>();
	if (0 != u5_resources->LoadResources())
	{
		return;
	}
	sdl_helper->LoadImageData(u5_resources.get());
	splash_screen = std::make_unique<SplashScreen>(sdl_helper.get(), u5_resources.get());
	splash_screen->LoadData();

	GameObject *curObject = splash_screen.get();
	
	while (1)
	{
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

	}
}

int main([[maybe_unused]]int argc, [[maybe_unused]] char** argv)
{
	bool valid;
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