#include "SplashScreen.h"

SplashScreen::SplashScreen(SDL3Helper *sdl_helper, UltimaVResource *u5_resources) :
	GameObject(sdl_helper, u5_resources)
{
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::Render()
{
	m_sdl_helper->Render();
}

void SplashScreen::LoadData()
{

}
