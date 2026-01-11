#pragma once

#include <SDL3/SDL.h>

#include "UltimaVResource.h"

class SDL3Helper
{
public:
	explicit SDL3Helper();

	int Intialize();
	void Cleanup();
	void Render();
	void Poll();
	void LoadImageData(UltimaVResource *u5_resources);
	void UpdateTicks();
	Uint64 GetCurrentTick();
	void ClearScreen();
	void RenderFlipTextureAt(SDL_Texture* texture, float x, float y, float width, float height, bool flip);
	void RenderTextureAt(SDL_Texture* texture, float x, float y, float width, float height);
	void RenderPresent();
	void GetScreenDimensions(int& width, int& height);

	bool m_quit;
	std::vector<std::vector<SDL_Texture*>> m_BitFileTextures;
	std::vector<std::vector<SDL_Texture*>> m_Image16FileTextures;
	SDL_Texture* m_PathFileTexture;
private:
	void LoadPathFileTexture(UltimaVResource* u5_resources);
	void LoadBitFileTextures(UltimaVResource* u5_resources);
	void LoadImage16FileTextures(UltimaVResource* u5_resources);
	void CreateTextureFromMemory(SDL_Texture*& texture, const U5ImageData& curData);

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Event m_event;
	
	Uint64 m_curTick;
};

