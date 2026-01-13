#pragma once

#include "UltimaVResource.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

enum TARGET_TEXTURE_VAL
{
	TTV_INTROBOX,
	TTV_INTROBOX_DISPLAY
};

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
	void ClearScreen() const;
	void RenderFlipTextureAt(SDL_Texture* texture, float x, float y, float width, float height, bool flip) const;
	void RenderTextureAt(SDL_Texture* texture, float x, float y, float width, float height) const;
	void RenderPresent() const;
	void GetScreenDimensions(int& width, int& height);
	void TurnOnPixels(SDL_Texture* texture, std::vector<int>& vec_pixels, bool on);
	void SetRenderTarget(SDL_Texture* texture) const;

	bool m_quit;
	SDL_Renderer* m_renderer;
	std::vector<std::vector<SDL_Texture*>> m_BitFileTextures;
	std::vector<std::vector<SDL_Texture*>> m_Image16FileTextures;
	std::vector<SDL_Texture*> m_TargetTextures;
	std::vector<std::vector<std::vector<SDL_Texture*>>> m_CharacterSetsTextures;
	std::vector<SDL_Texture*> m_ArrowTextures;
	SDL_Texture* m_PathFileTexture;
	SDL_Texture* m_LogoFadeTexture;
	SDL_Texture* m_Flame1FadeTexture;
	SDL_Texture* m_WoDFadeTexture;
private:
	void LoadPathFileTexture(UltimaVResource* u5_resources);
	void LoadBitFileTextures(UltimaVResource* u5_resources);
	void LoadCharacterSetTextures(UltimaVResource* u5_resources);
	void LoadImage16FileTextures(UltimaVResource* u5_resources);
	void CreateTextureFromMemory(SDL_Texture*& texture, const U5ImageData& curData) const;
	void LoadFadeTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha, bool has_transparent = false, unsigned char transparent_color[3] = {}) const;
	void LoadMaskTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha) const;
	void LoadTargetTextures();
	void CreateOutlineTexture(SDL_Texture*& texture, const U5ImageData& curData, unsigned char color_data[3]) const;

	SDL_Window* m_window;
	
	SDL_Event m_event;
	Uint64 m_curTick;
};

