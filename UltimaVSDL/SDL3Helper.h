#pragma once

#include "UltimaVResource.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <string>
#include <utility>
#include "U5Tile.h"

enum TARGET_TEXTURE_VAL
{
	TTV_MAIN_DISPLAY,
	TTV_MAIN_DISPLAY_BUFFER,
	TTV_INTROBOX,
	TTV_INTROBOX_DISPLAY,
	TTV_INTRO_RENDER,
	TTV_CONSOLE,
	TTV_CONSOLE_BUFFER
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
	void RenderTextureFromTo(SDL_Texture* texture, float from_x, float from_y, float from_width, float from_height,
		float to_x, float to_y, float to_width, float to_height) const;
	void RenderPresent() const;
	void GetScreenDimensions(int& width, int& height);
	void TurnOnPixels(SDL_Texture* texture, std::vector<int>& vec_pixels, bool on);
	void TurnOnAllPixels(SDL_Texture* texture, bool on);
	void SetRenderTarget(SDL_Texture* texture) const;
	void ClearStreamingTexture(SDL_Texture *texture);
	void CopyTextureToStreaming(U5ImageData &texture, SDL_Texture *streaming_texture, uint32_t width, uint32_t height, bool hasAlpha = false);
	void DrawTiledText(std::string text, int x_tile, int y_tile);
	void DrawInvertRect(int x_tile, int y_tile, int width, int height) const;
	void DrawTileTexture8(SDL_Texture* texture, int x_tile, int y_tile) const;
	void DrawTileTexture(SDL_Texture* texture, int x_tile, int y_tile) const;
	void DrawTileRect(int x_tile, int y_tile) const;
	void DrawTileRect(int x_tile, int y_tile, int num_x, int num_y, unsigned char color, RenderMode mode) const;
	void DrawLineManual(std::vector<std::pair<int, int>> m_rect, int step, bool isHorz, int xPos, int yPos) const;
	void CreateTextureFromMemory(SDL_Texture*& texture, const U5ImageData& curData, bool has_transparent = false, unsigned char transparent_color[3] = {}) const;
	void CreateTextureFromMemoryWithMask(SDL_Texture*& texture, SDL_Texture*& render_texture, const U5ImageData& curData, const U5ImageData& maskData) const;
	void AnimateTiles(Uint64 elapsed_time);

	bool m_quit;
	SDL_Renderer* m_renderer;
	std::vector<std::vector<SDL_Texture*>> m_BitFileTextures;
	std::vector<std::vector<SDL_Texture*>> m_Image16FileTextures;
	std::vector<SDL_Texture*> m_TargetTextures;
	std::vector<std::vector<std::vector<SDL_Texture*>>> m_CharacterSetsTextures;
	std::vector<SDL_Texture*> m_ArrowTextures;
	std::vector<SDL_Texture*> m_ProportionalFontTextures;

	std::vector<U5Tile> m_TileTextures;
	
	SDL_Texture* m_PathFileTexture;
	SDL_Texture* m_LogoFadeTexture;
	SDL_Texture* m_Flame1FadeTexture;
	SDL_Texture* m_WoDFadeTexture;
	SDL_Texture* m_FullScreenTexture;
	SDL_Texture* m_CodexFadeTexture;
	SDL_Texture* m_TileFadeTexture;
	SDL_Texture* m_PromptTexture;
private:
	void LoadTileTextures(UltimaVResource* u5_resources);
	void LoadPathFileTexture(UltimaVResource* u5_resources);
	void LoadBitFileTextures(UltimaVResource* u5_resources);
	void LoadCharacterSetTextures(UltimaVResource* u5_resources);
	void LoadImage16FileTextures(UltimaVResource* u5_resources);
	void LoadProportionalFontTextures(UltimaVResource* u5_resources);
	void LoadFadeTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha, bool has_transparent = false, unsigned char transparent_color[3] = {}) const;
	void LoadMaskTexture(U5ImageData& data, SDL_Texture*& texture, bool alpha) const;
	void LoadTargetTextures();
	void LoadStreamingTextures();
	void CreateOutlineTexture(SDL_Texture*& texture, const U5ImageData& curData, unsigned char color_data[3]) const;

	SDL_Window* m_window;
	
	SDL_Event m_event;
	Uint64 m_curTick;
};

