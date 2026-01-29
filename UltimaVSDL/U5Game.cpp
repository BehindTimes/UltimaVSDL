#include "U5Game.h"
#include "GameObject.h"
#include "UltimaVResource.h"
#include <memory>
#include "U5Input.h"
#include "U5Utils.h"
#include "SDL3Helper.h"

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

U5Game::U5Game(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_window_width(0),
	m_window_height(0)
{
}

U5Game::~U5Game()
{
}

void U5Game::LoadData()
{
	m_input->SetInputType(InputType::ANY_KEY);
}

void U5Game::SetSDLData()
{
	GameObject::SetSDLData();
}

void U5Game::Render()
{
	m_sdl_helper->SetRenderTarget(nullptr);
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();
	m_sdl_helper->RenderPresent();
}

void U5Game::ProcessEvents()
{
}
