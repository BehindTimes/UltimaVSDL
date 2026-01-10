#include "GameObject.h"

GameObject::GameObject(SDL3Helper *sdl_helper, UltimaVResource *u5_resources) :
	m_sdl_helper(sdl_helper),
	m_resources(u5_resources)
{
}

GameObject::~GameObject()
{
}

void GameObject::LoadData()
{

}