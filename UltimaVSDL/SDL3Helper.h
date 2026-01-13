#pragma once

#include "UltimaVResource.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

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
	Uint64 GetCurrentTick() const;
	void ClearScreen();
	void RenderFlipTextureAt(SDL_Texture* texture, float x, float y, float width, float height, bool flip);
	void RenderTextureAt(SDL_Texture* texture, float x, float y, float width, float height);
	void RenderPresent();
	void GetScreenDimensions(int& width, int& height);
	void TurnOnPixels(SDL_Texture* texture, std::vector<int>& vec_pixels, bool on);

	bool m_quit;
	std::vector<std::vector<SDL_Texture*>> m_BitFileTextures;
	std::vector<std::vector<SDL_Texture*>> m_Image16FileTextures;
	SDL_Texture* m_PathFileTexture;
	SDL_Texture* m_LogoFadeTexture;
	SDL_Texture* m_Flame1FadeTexture;
	SDL_Texture* m_WoDFadeTexture;
private:
	void LoadPathFileTexture(UltimaVResource* u5_resources);
	void LoadBitFileTextures(UltimaVResource* u5_resources);
	void LoadImage16FileTextures(UltimaVResource* u5_resources);
	void CreateTextureFromMemory(SDL_Texture*& texture, const U5ImageData& curData);
	void LoadFadeTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha, bool has_transparent = false, unsigned char transparent_color[3] = {});
	void LoadMaskTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha);

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Event m_event;
	
	Uint64 m_curTick;
};

