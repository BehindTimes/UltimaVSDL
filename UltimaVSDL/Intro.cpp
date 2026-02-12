#include "Intro.h"
#include "U5Utils.h"
#include <memory>
#include <vector>
#include <SDL3/SDL_render.h>
#include "GameObject.h"
#include "SDL3Helper.h"
#include "U5Enums.h"
#include "UltimaVResource.h"
#include "ColorData.h"
#include <SDL3/SDL_rect.h>
#include "FadeObject.h"
#include "CutScene.h"
#include <functional>
#include <string>
#include "U5Input.h"
#include <SDL3/SDL_keycode.h>
#include <cstring>
#include <SDL3/SDL_stdinc.h>
#include <iostream>

extern std::unique_ptr<CutScene> cutscene_screen;
extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;

Intro::Intro(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameObject(sdl_helper, u5_resources),
	m_curDelayFlame(0),
	m_curFlame(0),
	m_curMode(IntroMode::FADE_LOGO),
	m_window_width(0),
	m_window_height(0),
	m_curWodFade(0),
	m_curMenuIndex(0),
	m_curAcknowledgementYPos(0),
	m_curAcknowledgementXPos(0),
	m_curAcknowledgementYDelay(0),
	m_curAcknowledgementXDelay(0),
	m_curAcknowledgement(AcknowlegementType::SCROLL_UP),
	m_demoInstructionNum(0),
	m_curInstructionDelay(0),
	m_demoBackground(-1),
	m_demo_screen_open(false),
	m_curScreenOpenDelay(0),
	m_smoothDemoOpen(false),
	m_DemoMap{},
	m_curDelayTime(1),
	m_DemoFadeTileNum(-1),
	m_fadeIn(true),
	m_numLoops(0),
	m_loopPos(0),
	m_isZap(false),
	m_playerHit(-1)
{
	m_clearScreen = true;

	m_LogoFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_LogoFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][0].height * m_resources->m_Image16FileData[12][0].width));
	m_LogoFade->SetDelay(LOGO_FADE_DELAY);

	m_FlameFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_FlameFade->SetSize(static_cast<int>(m_resources->m_Image16FileData[12][1].height * m_resources->m_Image16FileData[12][1].width));
	m_FlameFade->SetDelay(FLAME_FADE_DELAY);

	m_WoDFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_WoDFade->SetSize(static_cast<size_t>(m_resources->m_BitFileData[2][0].height * m_resources->m_BitFileData[2][0].width));
	m_WoDFade->SetDelay(WOD_FADE_DELAY);

	m_DemoTileFade = std::make_unique<FadeObject>(sdl_helper, u5_resources);
	m_DemoTileFade->SetSize(static_cast<size_t>(m_resources->m_Tiles[0].height * m_resources->m_Tiles[0].width));
	m_DemoTileFade->SetDelay(TILE_FADE_DELAY);
}

Intro::~Intro()
{
}

void Intro::RenderWoD()
{
	float CGA_OFFSET = 0;
	float width;
	float height;

	SDL_Texture* curTexture;

	// Not sure why the Warriors of Destiny image is shifted 4 pixels over, but it is
	if (m_resources->m_render_mode == RenderMode::CGA)
	{
		CGA_OFFSET = -4;
	}

	height = static_cast<float>(m_resources->m_BitFileData[2][0].height);
	width = static_cast<float>(m_resources->m_BitFileData[2][0].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	curTexture = m_sdl_helper->m_WoDFadeTexture;

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, (x + CGA_OFFSET) * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderFlameFadeWoD()
{
	SDL_Texture* curTexture = m_sdl_helper->m_WoDFadeTexture;

	m_WoDFade->AddElapsedTime(m_tickElapse);
	if (!m_WoDFade->IsFading())
	{
		//GoToSelection();
		CreateDemo();
	}
	m_WoDFade->ProcessFade(curTexture, false);
}

void Intro::RenderFlameFadeIn1()
{
	float width;
	float height;
	height = static_cast<float>(m_resources->m_Image16FileData[12][1].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][1].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	SDL_Texture* curTexture = m_sdl_helper->m_Flame1FadeTexture;

	m_FlameFade->AddElapsedTime(m_tickElapse);
	if (!m_FlameFade->IsFading())
	{
		m_curMode = IntroMode::FADE_FLAME_2;
	}
	bool done = m_FlameFade->ProcessFade(curTexture, false);
	if (done)
	{
		return;
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 64;

	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderLogoFadeIn()
{
	float width;
	float height;
	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	SDL_Texture* curTexture = m_sdl_helper->m_LogoFadeTexture;

	m_LogoFade->AddElapsedTime(m_tickElapse);
	if (!m_LogoFade->IsFading())
	{
		m_curMode = IntroMode::FADE_FLAME_1;
		RenderLogo();
		return;
	}
	bool done = m_LogoFade->ProcessFade(curTexture);
	if (done)
	{
		RenderLogo();
		return;
	}

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::RenderLogo()
{
	float width;
	float height;
	
	SDL_Texture* curTexture;
	
	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	height = static_cast<float>(m_resources->m_Image16FileData[12][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][0].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[12][0]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 0;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}


void Intro::RenderFlame()
{
	m_curDelayFlame += m_tickElapse;
	if (FLAME_DELAY < m_curDelayFlame)
	{
		m_curDelayFlame %= FLAME_DELAY;
		m_curFlame++;
		m_curFlame %= 4;
	}

	float width;
	float height;
	int window_width, window_height;
	SDL_Texture* curTexture;

	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	height = static_cast<float>(m_resources->m_Image16FileData[12][static_cast<size_t>(m_curFlame + 1)].height);
	width = static_cast<float>(m_resources->m_Image16FileData[12][static_cast<size_t>(m_curFlame + 1)].width);

	curTexture = m_sdl_helper->m_Image16FileTextures[12][static_cast<size_t>(m_curFlame + 1)]; // A

	float x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	float y = 0;

	x = (ORIGINAL_GAME_WIDTH - width) / 2.0f;
	y = 64;
	m_sdl_helper->RenderTextureAt(curTexture, x * hMult, y * vMult, width * hMult, height * vMult);
}

void Intro::CreateDemo()
{
	m_curMode = IntroMode::DEMO;
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTRO_RENDER];
	m_sdl_helper->SetRenderTarget(curTexture);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0x00, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();
	m_sdl_helper->SetRenderTarget(nullptr);
	m_demoInstructionNum = 0;
	m_curInstructionDelay = 0;
	m_curScreenOpenDelay = 0;
	m_demo_screen_open = false;
	m_demoBackground = -1;
	m_curInstructionDelay = 0;
	m_curDelayTime = 1;
	m_DemoFadeTileNum = -1;
	m_playerHit = -1;
	m_moongate.m_showMoongate = MoongateStatus::CLOSED;

	//m_demoInstructionNum = 90;
	//m_demoInstructionNum = 180;
}

void Intro::CreateMenu()
{
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY];
	m_sdl_helper->SetRenderTarget(curTexture);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0x00, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();
	const int NUM_X_TILES = 40;
	const int Y_TILE_START = 1;
	int invertx = 0;
	int inverty = 0;
	int invertindex = m_curMenuIndex;
	int invert_width = 0;

	for (int index = 0; index < 6; index++)
	{
		std::string curVal = m_resources->m_data.intro_strings[index];
		if (curVal.size() > NUM_X_TILES)
		{
			continue;
		}
		int start_x_tile = (NUM_X_TILES - static_cast<int>(curVal.size())) / 2;
		m_sdl_helper->DrawTiledText(curVal, start_x_tile, Y_TILE_START + index);

		if (invertindex == index)
		{
			invertx = start_x_tile;
			inverty = Y_TILE_START + index;// +16;
			invert_width = static_cast<int>(curVal.size() + 2);
		}
	}
	m_sdl_helper->DrawInvertRect(invertx - 1, inverty, invert_width, 1);

	curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX];
	m_sdl_helper->SetRenderTarget(curTexture);
	m_sdl_helper->DrawTiledText(m_resources->m_data.intro_strings[12], 6, 9); // Copyright Lord British
	m_sdl_helper->DrawTiledText(m_resources->m_data.intro_strings[13], 16, 0); // Select

	m_sdl_helper->DrawTileRect(24, 0);
	m_sdl_helper->DrawTileRect(33, 9);
	m_sdl_helper->DrawTileRect(15, 0);
	m_sdl_helper->DrawTileRect(5, 9);

	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 24, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 33, 9);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 15, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 5, 9);

	m_sdl_helper->SetRenderTarget(nullptr);
	//m_sdl_helper->DrawInvertRect(invertx, inverty, invert_width, 1);	
}

void Intro::RenderCursor()
{
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>(5 + m_curFlame)], 23, 15);
}

void Intro::RenderMenu()
{
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY];

	float x = HALF_TILE_HEIGHT;
	float y = 16 * HALF_TILE_HEIGHT;
	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH - RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * 4);
}

void Intro::RenderIntroBox()
{
	SDL_Texture* curTexture;

	float x = 0;
	float y = 15 * HALF_TILE_HEIGHT;
	curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX];
	m_sdl_helper->RenderTextureAt(curTexture, x, y, RENDER_WIDTH, RENDER_TILE_HEIGHT * 5);
}

void Intro::RenderDemo()
{
	SDL_Texture* dispTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY];
	SDL_Texture* curTexture = m_sdl_helper->m_TargetTextures[TTV_INTRO_RENDER];
	SDL_Texture* fadeTexture = m_sdl_helper->m_TileFadeTexture;

	m_curInstructionDelay += m_tickElapse;
	if (m_curDelayTime > 0 && m_curDelayTime < m_curInstructionDelay)
	{
		if (m_isZap)
		{
			m_isZap = false;
		}
		ProcessDemoScript();
	}
	if (m_demo_screen_open)
	{
		m_curScreenOpenDelay += m_tickElapse;
		if (m_curScreenOpenDelay > SCREEN_OPEN_DELAY)
		{
			m_demo_screen_open = false;
		}
	}
	if (m_moongate.m_showMoongate == MoongateStatus::OPENING)
	{
		m_moongate.m_curMoongateDelay += m_tickElapse;
		if (m_moongate.m_curMoongateDelay > DemoMoongate::MOONGATE_DELAY)
		{
			m_moongate.m_showMoongate = MoongateStatus::OPENED;
		}
	}
	else if (m_moongate.m_showMoongate == MoongateStatus::CLOSING)
	{
		m_moongate.m_curMoongateDelay += m_tickElapse;
		if (m_moongate.m_curMoongateDelay > DemoMoongate::MOONGATE_DELAY)
		{
			m_moongate.m_showMoongate = MoongateStatus::CLOSED;
		}
	}

	if (m_DemoFadeTileNum >= 0)
	{
		m_DemoTileFade->AddElapsedTime(m_tickElapse);
		if (!m_DemoTileFade->IsFading())
		{
			if (m_map_demo_char.contains(m_DemoFadeTileNum))
			{
				m_map_demo_char[m_DemoFadeTileNum].isFade = false;
				if (!m_fadeIn)
				{
					m_map_demo_char[m_DemoFadeTileNum].isVisible = false;
				}
			}
			m_DemoFadeTileNum = -1;
		}
		else
		{
			m_DemoTileFade->ProcessFade(fadeTexture, m_fadeIn);
		}
	}

	if (m_demoBackground < 0 || m_demoBackground > 3)
	{
		m_sdl_helper->SetRenderTarget(nullptr);
		float x = HALF_TILE_HEIGHT;
		float y = 16 * HALF_TILE_HEIGHT;
		m_sdl_helper->RenderTextureAt(dispTexture, x, y, RENDER_WIDTH - RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * 4);
		return;
	}
	m_sdl_helper->SetRenderTarget(curTexture);
	// Render the base map
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			m_sdl_helper->DrawTileTexture(m_sdl_helper->m_TileTextures[m_DemoMap[j][i]].GetTexture(), j, i);
		}
	}

	// Render the moongate if available
	if (m_moongate.m_showMoongate == MoongateStatus::OPENING)
	{
		float ratio = static_cast<float>(m_moongate.m_curMoongateDelay) / DemoMoongate::MOONGATE_DELAY;
		m_sdl_helper->RenderTextureFromTo(m_sdl_helper->m_TileTextures[m_moongate.TILE_NUM].GetTexture(), 0, 0,
			ORIGINAL_TILE_WIDTH, ORIGINAL_TILE_HEIGHT * ratio, m_moongate.x * static_cast<float>(RENDER_TILE_WIDTH),
			static_cast<float>(m_moongate.y * RENDER_TILE_HEIGHT) + (RENDER_TILE_HEIGHT * (1 - ratio)), RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * ratio);
	}
	else if (m_moongate.m_showMoongate == MoongateStatus::CLOSING)
	{
		float ratio = 1.0f - (static_cast<float>(m_moongate.m_curMoongateDelay) / DemoMoongate::MOONGATE_DELAY);
		if (ratio > 0)
		{
			m_sdl_helper->RenderTextureFromTo(m_sdl_helper->m_TileTextures[m_moongate.TILE_NUM].GetTexture(), 0, 0,
				ORIGINAL_TILE_WIDTH, ORIGINAL_TILE_HEIGHT * ratio, m_moongate.x * static_cast<float>(RENDER_TILE_WIDTH),
				static_cast<float>(m_moongate.y * RENDER_TILE_HEIGHT) + (RENDER_TILE_HEIGHT * (1 - ratio)), RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * ratio);
		}
	}
	else if (m_moongate.m_showMoongate == MoongateStatus::OPENED)
	{
		m_sdl_helper->DrawTileTexture(m_sdl_helper->m_TileTextures[m_moongate.TILE_NUM].GetTexture(), m_moongate.x, m_moongate.y);
	}

	// Render the tiles to draw above
	for (auto& curTile : m_map_demo_char)
	{
		int curTextureNum = 0x100;
		if (curTile.second.tile >= 0 && curTile.second.tile < 0x100)
		{
			curTextureNum += curTile.second.tile;
		}
		if (curTile.second.isVisible)
		{
			if (curTile.second.isFade)
			{
				m_sdl_helper->DrawTileTexture(m_sdl_helper->m_TileFadeTexture, curTile.second.x, curTile.second.y);
			}
			else
			{
				m_sdl_helper->DrawTileTexture(m_sdl_helper->m_TileTextures[curTextureNum].GetTexture(), curTile.second.x, curTile.second.y);
			}
		}
	}
	// Draw the Shadowlord zap here
	if (m_isZap)
	{
		const Uint64 ZAP_TIME = 100;
		const Uint64 HIT_DELAY = 50;
		Uint64 curTime = m_curInstructionDelay;
		int zap_num = static_cast<int>(m_curInstructionDelay / ZAP_TIME);
		Uint64 tempTime = curTime - (zap_num * ZAP_TIME);

		if (zap_num < HIT_DELAY)
		{
			if (tempTime < 50)
			{
				if (m_map_demo_char.contains(m_playerHit))
				{
					//std::cout << zap_num << std::endl;
					m_sdl_helper->DrawLineManual({ {8,8}, {12,8}, {12,8}, {8,8} }, 4, true, 480 + (36 * zap_num), 92 + (12 * zap_num));
					if (zap_num == 4)
					{
						m_sdl_helper->DrawTileTexture(m_sdl_helper->m_TileTextures[0].GetTexture(), m_map_demo_char[m_playerHit].x, m_map_demo_char[m_playerHit].y);
					}
				}
			}
		}
	}

	m_sdl_helper->SetRenderTarget(dispTexture);
	if (m_demo_screen_open)
	{
		float ratio = static_cast<float>(m_curScreenOpenDelay) / SCREEN_OPEN_DELAY;
		float final_width = (RENDER_WIDTH - RENDER_TILE_WIDTH) * ratio;
		float start_x;
		if (m_smoothDemoOpen)
		{
			start_x = ((RENDER_WIDTH - RENDER_TILE_WIDTH) / 2.0f) - (final_width / 2.0f);
			m_sdl_helper->RenderTextureFromTo(curTexture, start_x, 0, final_width, RENDER_TILE_HEIGHT * 4, start_x, 0, final_width, RENDER_TILE_HEIGHT * 4);
		}
		else
		{
			int numTiles = static_cast<int>(final_width / (RENDER_TILE_WIDTH));
			if (numTiles % 2 == 0)
			{
				numTiles--;
			}
			final_width = static_cast<float>(numTiles * RENDER_TILE_WIDTH);
			start_x = ((RENDER_WIDTH - RENDER_TILE_WIDTH) / 2.0f) - (final_width / 2.0f);
			m_sdl_helper->RenderTextureFromTo(curTexture, start_x, 0, final_width, RENDER_TILE_HEIGHT * 4, start_x, 0, final_width, RENDER_TILE_HEIGHT * 4);
		}
	}
	else
	{
		m_sdl_helper->RenderTextureAt(curTexture, 0, 0, RENDER_WIDTH - RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * 4);
	}
	
	m_sdl_helper->SetRenderTarget(nullptr);
	float x = HALF_TILE_HEIGHT;
	float y = 16 * HALF_TILE_HEIGHT;
	m_sdl_helper->RenderTextureAt(dispTexture, x, y, RENDER_WIDTH - RENDER_TILE_WIDTH, RENDER_TILE_HEIGHT * 4);

	m_sdl_helper->AnimateTiles(m_tickElapse);

	// Control the NPC animations individually
	for (auto& curNPC : m_map_demo_char)
	{
		if (curNPC.second.tile >= 52)
		{
			if (curNPC.second.tile >= 180 && curNPC.second.tile <= 183)
			{
				return;
			}
			curNPC.second.m_curNPCDelay += m_tickElapse;
			bool changeTile = false;
			if (curNPC.second.m_curNPCDelay >= NPC_ANIMATE)
			{
				curNPC.second.m_curNPCDelay %= NPC_ANIMATE;
				changeTile = true;
			}
			if (changeTile)
			{
				int startTile = curNPC.second.tile - (curNPC.second.tile % 4);
				int endTile = startTile + 3;

				int val = m_utilities->GetRandom(0, 2);
				if (val == 0)
				{
					if (curNPC.second.tile > 0)
					{
						curNPC.second.tile--;
						if (curNPC.second.tile < startTile)
						{
							curNPC.second.tile = startTile;
						}
					}
				}
				else if (val == 1)
				{
					if (curNPC.second.tile < 0xFF)
					{
						curNPC.second.tile++;
						if (curNPC.second.tile > endTile)
						{
							curNPC.second.tile = endTile;
						}
					}
				}
			}
		}
	}
}

void Intro::RenderAcknowledgements()
{
	const int TILE_LEFT_START = 9 * RENDER_TILE_WIDTH;
	const int TILE_RIGHT_START = 10 * RENDER_TILE_WIDTH;

	float width;
	float height;

	SDL_Texture* curTexture;
	SDL_Texture* paperTexture = nullptr;

	float vMult = m_window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = m_window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	height = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][0].height);
	width = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][0].width);

	float paperW = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][1].width);
	float paperH = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][1].height);

	float yPosLeft = RENDER_HEIGHT;
	float yPosRight = RENDER_HEIGHT;

	float xPosLeft = TILE_LEFT_START;
	float xPosRight = TILE_RIGHT_START;

	float paperX = 0;

	float paperXLoc = 0;

	switch (m_curAcknowledgement)
	{
	case AcknowlegementType::SCROLL_UP:
		m_curAcknowledgementYDelay += m_tickElapse;
		if (m_curAcknowledgementYDelay > ACKNOWLEDGEMENT_SCROLL_DELAY)
		{
			m_curAcknowledgement = AcknowlegementType::OPEN;
			yPosLeft = RENDER_HEIGHT - (height * vMult);
			yPosRight = RENDER_HEIGHT - (height * vMult);
			m_curAcknowledgementXDelay = 0;
			m_curAcknowledgementYDelay = 0;
		}
		else
		{
			float scroll_ratio = static_cast<float>(m_curAcknowledgementYDelay) / ACKNOWLEDGEMENT_SCROLL_DELAY;
			yPosLeft = RENDER_HEIGHT - ((height * vMult) * scroll_ratio);
			yPosRight = RENDER_HEIGHT - ((height * vMult) * scroll_ratio);
		}
		break;
	case AcknowlegementType::SCROLL_DOWN:
		m_curAcknowledgementYDelay += m_tickElapse;
		if (m_curAcknowledgementYDelay > ACKNOWLEDGEMENT_SCROLL_DELAY)
		{
			m_curAcknowledgement = AcknowlegementType::SCROLL_UP;
			yPosLeft = RENDER_HEIGHT;
			yPosRight = RENDER_HEIGHT;
			m_curAcknowledgementXDelay = 0;
			m_curAcknowledgementYDelay = 0;
			GoToSelection();
		}
		else
		{
			float scroll_ratio = 1.0f - static_cast<float>(m_curAcknowledgementYDelay) / ACKNOWLEDGEMENT_SCROLL_DELAY;
			yPosLeft = RENDER_HEIGHT - ((height * vMult) * scroll_ratio);
			yPosRight = RENDER_HEIGHT - ((height * vMult) * scroll_ratio);
		}
		break;
	case AcknowlegementType::OPEN:
		yPosLeft = RENDER_HEIGHT - (height * vMult);
		yPosRight = RENDER_HEIGHT - (height * vMult);
		
		paperTexture = m_sdl_helper->m_Image16FileTextures[IV16_STARTSC][1];
		m_curAcknowledgementXDelay += m_tickElapse;
		if (m_curAcknowledgementXDelay > ACKNOWLEDGEMENT_OPEN_DELAY)
		{
			m_curAcknowledgement = AcknowlegementType::WAIT;
			m_curAcknowledgementXDelay = 0;
			m_curAcknowledgementYDelay = 0;
			xPosLeft = 0;
			xPosRight = RENDER_WIDTH - RENDER_TILE_WIDTH;
			paperX = xPosLeft + RENDER_TILE_WIDTH;
			height = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][0].height);
			width = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][0].width);
		}
		else
		{
			float scroll_ratio = static_cast<float>(m_curAcknowledgementXDelay) / ACKNOWLEDGEMENT_OPEN_DELAY;
			xPosLeft = TILE_LEFT_START - (TILE_LEFT_START * scroll_ratio);
			xPosRight = TILE_RIGHT_START + ((TILE_RIGHT_START - RENDER_TILE_WIDTH) * scroll_ratio);
			paperX = xPosLeft + RENDER_TILE_WIDTH;

			float tempW = paperW * scroll_ratio;
			float offset = paperW - tempW;
			offset /= 2.0f;
			paperW = tempW;
			paperXLoc = offset;
		}
		break;
	case AcknowlegementType::CLOSE:
		yPosLeft = RENDER_HEIGHT - (height * vMult);
		yPosRight = RENDER_HEIGHT - (height * vMult);
		m_curAcknowledgementXDelay += m_tickElapse;
		
		if (m_curAcknowledgementXDelay > ACKNOWLEDGEMENT_OPEN_DELAY)
		{
			m_curAcknowledgement = AcknowlegementType::SCROLL_DOWN;
			m_curAcknowledgementXDelay = 0;
			m_curAcknowledgementYDelay = 0;
			xPosLeft = TILE_LEFT_START;
			xPosRight = TILE_RIGHT_START;
		}
		else
		{
			paperTexture = m_sdl_helper->m_Image16FileTextures[IV16_STARTSC][1];
			float scroll_ratio = 1.0f - static_cast<float>(m_curAcknowledgementXDelay) / ACKNOWLEDGEMENT_OPEN_DELAY;
			xPosLeft = TILE_LEFT_START - (TILE_LEFT_START * scroll_ratio);
			xPosRight = TILE_RIGHT_START + ((TILE_RIGHT_START - RENDER_TILE_WIDTH) * scroll_ratio);
			paperX = xPosLeft + RENDER_TILE_WIDTH;

			float tempW = paperW * scroll_ratio;
			float offset = paperW - tempW;
			offset /= 2.0f;
			paperW = tempW;
			paperXLoc = offset;
		}
		break;
	case AcknowlegementType::WAIT:
		yPosLeft = RENDER_HEIGHT - (height * vMult);
		yPosRight = RENDER_HEIGHT - (height * vMult);
		xPosLeft = 0;
		xPosRight = RENDER_WIDTH - RENDER_TILE_WIDTH;
		paperX = xPosLeft + RENDER_TILE_WIDTH;
		paperW = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][1].width);
		paperH = static_cast<float>(m_resources->m_Image16FileData[IV16_STARTSC][1].height);
		paperTexture = m_sdl_helper->m_Image16FileTextures[IV16_STARTSC][1];;
		m_curAcknowledgementXDelay = 0;
		m_curAcknowledgementYDelay = 0;
		break;
	default:
		break;
	}

	if (paperW != 0 && paperTexture != nullptr)
	{
		//m_sdl_helper->RenderTextureAt(curTexture, paperX, yPosLeft, paperW * hMult, height * vMult);
		m_sdl_helper->RenderTextureFromTo(paperTexture, paperXLoc, 0, paperW, paperH, paperX, yPosLeft, paperW * hMult, height * vMult);
	}

	curTexture = m_sdl_helper->m_Image16FileTextures[IV16_STARTSC][0];
	m_sdl_helper->RenderTextureAt(curTexture, xPosLeft, yPosLeft, width * hMult, height * vMult);
	curTexture = m_sdl_helper->m_Image16FileTextures[IV16_STARTSC][2];
	m_sdl_helper->RenderTextureAt(curTexture, xPosRight, yPosRight, width * hMult, height * vMult);
}

void Intro::Render()
{
	m_sdl_helper->GetScreenDimensions(m_window_width, m_window_height);
	m_sdl_helper->ClearScreen();

	switch (m_curMode)
	{
	case IntroMode::FADE_LOGO:
		RenderLogoFadeIn();
		break;
	case IntroMode::FADE_FLAME_1:
		RenderLogo();
		RenderFlame();
		RenderFlameFadeIn1();
		RenderWoD();
		break;
	case IntroMode::FADE_FLAME_2:
		RenderLogo();
		RenderFlame();
		RenderFlameFadeWoD();
		RenderWoD();
		break;
	case IntroMode::ACKNOWLEDGEMENTS:
		RenderLogo();
		RenderFlame();
		RenderIntroBox();
		RenderMenu();
		RenderAcknowledgements();
		break;
	case IntroMode::DEMO:
		RenderLogo();
		RenderFlame();
		RenderIntroBox();
		RenderDemo();
		break;
	default:
		RenderLogo();
		RenderFlame();
		RenderIntroBox();
		RenderMenu();
		RenderCursor();
		break;
	}

	m_sdl_helper->RenderPresent();
}

void Intro::LoadData()
{
	SetSDLData();
	CreateIntroBox();
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0x00, 0x00, 0x00, 0xFF);
	m_sdl_helper->ClearScreen();
	m_sdl_helper->SetRenderTarget(nullptr);
	//CreateMenu();
	//m_input->SetInputType(InputType::ANY_KEY);
	m_input->SetInputType(InputType::UP_DOWN_ENTER);
}

void Intro::SetSDLData()
{
	GameObject::SetSDLData();
}

void Intro::CreateIntroBox()
{
	SDL_Texture* curTexture;
	const int blue = 1;
	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX]);
	if (m_resources->m_render_mode == RenderMode::CGA) // CGA
	{
		SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, cga_table[blue][0], cga_table[blue][1], cga_table[blue][2], 0xFF);
	}
	else
	{
		SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, ega_table[blue][0], ega_table[blue][1], ega_table[blue][2], 0xFF);
	}

	m_sdl_helper->ClearScreen();
	//m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX_DISPLAY]);
	//SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0x00, 0x00, 0x00, 0xFF);
	//m_sdl_helper->ClearScreen();

	float x = 0;
	float y = 15 * HALF_TILE_HEIGHT;

	if (m_resources->m_render_mode == RenderMode::CGA) // CGA
	{
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][123], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][124], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][125], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][126], cga_table[blue][0], cga_table[blue][1], cga_table[blue][2]);
	}
	else
	{
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][123], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][124], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][125], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
		SDL_SetTextureColorMod(m_sdl_helper->m_CharacterSetsTextures[0][0][126], ega_table[blue][0], ega_table[blue][1], ega_table[blue][2]);
	}

	//m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_INTROBOX]);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][123];
	m_sdl_helper->RenderTextureAt(curTexture, 0, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][124];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, 0, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][125];
	m_sdl_helper->RenderTextureAt(curTexture, 0, HALF_TILE_HEIGHT * 9, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);
	curTexture = m_sdl_helper->m_CharacterSetsTextures[0][0][126];
	m_sdl_helper->RenderTextureAt(curTexture, RENDER_WIDTH - HALF_TILE_WIDTH, HALF_TILE_HEIGHT * 9, HALF_TILE_WIDTH, HALF_TILE_HEIGHT);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_FRect line{ HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, RENDER_WIDTH - RENDER_TILE_WIDTH + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, (RENDER_TILE_HEIGHT * 4) + HALF_TILE_HEIGHT, RENDER_WIDTH - RENDER_TILE_WIDTH + (2 * LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT, LINE_THICKNESS, (RENDER_TILE_HEIGHT * 4) };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { RENDER_WIDTH - HALF_TILE_WIDTH, HALF_TILE_HEIGHT, LINE_THICKNESS, (RENDER_TILE_HEIGHT * 4) };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	x = HALF_TILE_WIDTH;
	y = HALF_TILE_HEIGHT;

	line = { HALF_TILE_WIDTH, HALF_TILE_HEIGHT, RENDER_WIDTH - RENDER_TILE_WIDTH, (RENDER_TILE_HEIGHT * 4) };
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	m_sdl_helper->SetRenderTarget(nullptr);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0);
}

void Intro::GoToSelection()
{
	m_curMode = IntroMode::MENU;
	CreateMenu();
}

bool Intro::ChangeMode(U5Modes& newMode)
{
	bool ret = GameObject::ChangeMode(newMode);

	if (newMode == U5Modes::Cutscene)
	{
		cutscene_screen->SetCutScreenInfo(U5Modes::MenuSkip, m_resources->m_data.story_text, IV16_STORY1, std::bind(&Intro::StoryOverCallback, this));
		//cutscene_screen->SetCutScreenInfo(U5Modes::MenuSkip, m_resources->m_data.ending_text, IV16_END1, std::bind(&Intro::StoryOverCallback, this));
	}

	return ret;
}

void Intro::StoryOverCallback()
{
	m_input->SetInputType(InputType::UP_DOWN_ENTER);
	GoToSelection();
}

void Intro::ProcessEvents()
{
	switch (m_curMode)
	{
	case IntroMode::FADE_LOGO:
	case IntroMode::FADE_FLAME_1:
	case IntroMode::FADE_FLAME_2:
	case IntroMode::DEMO:
		if (m_input->isAnyKeyHit())
		{
			m_newMode = U5Modes::MenuSkip;
			m_input->SetRequireAllKeysUp();
		}
		break;
	case IntroMode::ACKNOWLEDGEMENTS:
		if (m_input->isAnyKeyHit())
		{
			m_input->SetRequireAllKeysUp();
			m_curAcknowledgementXDelay = 0;
			m_curAcknowledgementYDelay = 0;

			switch (m_curAcknowledgement)
			{
			case AcknowlegementType::SCROLL_UP:
			case AcknowlegementType::OPEN:
				m_curAcknowledgement = AcknowlegementType::WAIT;
				break;
			case AcknowlegementType::WAIT:
				m_curAcknowledgement = AcknowlegementType::CLOSE;
				break;
			case AcknowlegementType::CLOSE:
			case AcknowlegementType::SCROLL_DOWN:
				m_curAcknowledgement = AcknowlegementType::SCROLL_UP;
				GoToSelection();
				break;
			default:
				break;
			}
		}
		break;
	case IntroMode::MENU:
		if (m_input->isAnyKeyHit())
		{
			SDL_Keycode curKey = m_input->GetKeyCodePressed();
			switch (curKey)
			{
			case SDLK_UP:
				m_curMenuIndex--;
				if (m_curMenuIndex < 0)
				{
					m_curMenuIndex = 5;
				}
				break;
			case SDLK_DOWN:
				m_curMenuIndex++;
				if (m_curMenuIndex > 5)
				{
					m_curMenuIndex = 0;
				}
				break;
			case SDLK_RETURN:
				switch (static_cast<MenuChoices>(m_curMenuIndex))
				{
				case MenuChoices::ACKNOWLEDGEMENTS:
					m_curMode = IntroMode::ACKNOWLEDGEMENTS;
					m_curAcknowledgement = AcknowlegementType::SCROLL_UP;
					m_curAcknowledgementYDelay = 0;
					m_curAcknowledgementXDelay = 0;
					m_input->SetRequireAllKeysUp();
					break;
				case MenuChoices::INTRODUCTION: // Introduction
					m_newMode = U5Modes::Cutscene;
					m_input->SetRequireAllKeysUp();
					break;
				case MenuChoices::RETURN_TO_VIEW:
					CreateDemo();
					m_input->SetRequireAllKeysUp();
					break;
				case MenuChoices::JOURNEY_ONWARD:
					m_newMode = U5Modes::Game;
					m_input->SetRequireAllKeysUp();
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			CreateMenu();
		}
		break;
	default:
		break;
	}
}

void Intro::ProcessDemoScript()
{
	SDL_Texture* curTexture;
	int char_num;
	if (m_resources->m_IntroInstructions.size() <= m_demoInstructionNum)
	{
		m_curInstructionDelay = 0;
		return;
	}

	bool done = false;
	while (!done)
	{
		IntroScriptInstruction curInstruction = m_resources->m_IntroInstructions[m_demoInstructionNum];
		switch (curInstruction.instruction)
		{
		case 0x0:
			m_map_demo_char[curInstruction.data[0]] = DemoCharacter(curInstruction.data[1], curInstruction.data[2], curInstruction.data[3]);
			m_demoInstructionNum++;
			break;
		case 0x01:
			char_num = curInstruction.data[0];
			m_map_demo_char.erase(char_num);
			m_demoInstructionNum++;
			break;
		case 0x2: // Move character and run next instruction
			m_demoInstructionNum++;
			m_curDelayTime = SCRIPT_TICK * 6;
			m_curInstructionDelay = 0;
			char_num = curInstruction.data[0];
			if (m_map_demo_char.contains(char_num))
			{
				// Fortunately, because they are based on bytes, they can never exceed an int value
				switch (curInstruction.data[1])
				{
				case 0: // Up
					m_map_demo_char[char_num].y--;
					break;
				case 1: // Right
					m_map_demo_char[char_num].x++;
					break;
				case 2: // Down
					m_map_demo_char[char_num].y++;
					break;
				case 3: // Left
					m_map_demo_char[char_num].x--;
					break;
				default:
					break;
				}
			}
			break;
		case 0x3: // Delay
			m_demoInstructionNum++;
			done = true;
			m_curDelayTime = SCRIPT_TICK * curInstruction.data[0];
			m_curInstructionDelay = 0;
			if (0)
			{
				if (curInstruction.data[0] > 5)
				{
					m_curDelayTime = SCRIPT_TICK * 5; // Just for debug purposes to speed things up
				}
			}
			break;
		case 0x4: // Show moongate
			m_demoInstructionNum++;
			m_curDelayTime = DemoMoongate::MOONGATE_DELAY;
			m_curInstructionDelay = 0;
			m_moongate.m_curMoongateDelay = 0;
			done = true;
			m_moongate.m_showMoongate = MoongateStatus::OPENING;
			m_moongate.x = curInstruction.data[0];
			m_moongate.y = curInstruction.data[1];
			break;
		case 0x5: // Hide moongate
			m_demoInstructionNum++;
			m_curDelayTime = DemoMoongate::MOONGATE_DELAY;
			m_curInstructionDelay = 0;
			m_moongate.m_curMoongateDelay = 0;
			done = true;
			m_moongate.m_showMoongate = MoongateStatus::CLOSING;
			break;
		case 0x6:
			m_demoBackground = curInstruction.data[0];
			m_demo_screen_open = true;
			m_curScreenOpenDelay = 0;
			m_demoInstructionNum++;
			CreateIntroBox();
			curTexture = m_sdl_helper->m_TargetTextures[TTV_INTROBOX];
			if (m_demoBackground >= 0 && m_demoBackground <= 3)
			{
				int xpos = static_cast<int>(20 - ((m_resources->m_data.intro_demo_string[m_demoBackground].size() + 1) / 2));
				m_sdl_helper->SetRenderTarget(curTexture);
				m_sdl_helper->DrawTiledText(m_resources->m_data.intro_demo_string[m_demoBackground], xpos, 9);

				m_sdl_helper->DrawTileRect(xpos + static_cast<int>(m_resources->m_data.intro_demo_string[m_demoBackground].size()), 9);
				m_sdl_helper->DrawTileRect(xpos - 1, 9);

				m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], xpos + static_cast<int>(m_resources->m_data.intro_demo_string[m_demoBackground].size()), 9);
				m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], xpos - 1, 9);
				m_sdl_helper->SetRenderTarget(nullptr);
				std::memcpy(m_DemoMap, m_resources->m_DemoMap[m_demoBackground], sizeof(int) * DEMO_WIDTH * DEMO_HEIGHT);
			}
			else
			{
				std::memset(m_DemoMap, 0, sizeof(int) * DEMO_WIDTH * DEMO_HEIGHT);
			}
			break;
		case 0x07: // Fade in
			char_num = curInstruction.data[0];
			m_demoInstructionNum++;
			m_curDelayTime = TILE_FADE_DELAY;
			m_curInstructionDelay = 0;
			m_fadeIn = true;
			if (m_map_demo_char.contains(char_num))
			{
				m_DemoTileFade->Reset();
				m_map_demo_char[char_num].isFade = true;
				m_DemoFadeTileNum = char_num;
				int curTextureNum = 0x100;
				if (m_map_demo_char[char_num].tile >= 0 && m_map_demo_char[char_num].tile < 0x100)
				{
					curTextureNum += m_map_demo_char[char_num].tile;
				}
				m_sdl_helper->CopyTextureToStreaming(m_resources->m_Tiles[curTextureNum],
					m_sdl_helper->m_TileFadeTexture, ORIGINAL_TILE_WIDTH, ORIGINAL_TILE_HEIGHT);
			}
			done = true;
			break;
		case 0x08: // Fade out
			char_num = curInstruction.data[0];
			m_demoInstructionNum++;
			m_curDelayTime = TILE_FADE_DELAY;
			m_curInstructionDelay = 0;
			m_fadeIn = false;
			if (m_map_demo_char.contains(char_num))
			{
				m_DemoTileFade->Reset();
				m_map_demo_char[char_num].isFade = true;
				m_DemoFadeTileNum = char_num;
				int curTextureNum = 0x100;
				if (m_map_demo_char[char_num].tile >= 0 && m_map_demo_char[char_num].tile < 0x100)
				{
					//m_map_demo_char[char_num].isVisible = false;
					curTextureNum += m_map_demo_char[char_num].tile;
				}
				m_sdl_helper->CopyTextureToStreaming(m_resources->m_Tiles[curTextureNum],
					m_sdl_helper->m_TileFadeTexture, ORIGINAL_TILE_WIDTH, ORIGINAL_TILE_HEIGHT);
				m_sdl_helper->TurnOnAllPixels(m_sdl_helper->m_TileFadeTexture, true);
			}
			done = true;
			break;
		case 0x9: // Reset
			m_demoInstructionNum = 0;
			break;
		case 0xA: // Load tile
			m_demoInstructionNum++;
			m_DemoMap[curInstruction.data[1]][curInstruction.data[2]] = curInstruction.data[0];
			break;
		case 0xB: // Attack!
			m_demoInstructionNum++;
			m_curDelayTime = ZAP_DELAY;
			m_curInstructionDelay = 0;
			m_isZap = true;
			m_playerHit = curInstruction.data[1];
			done = true;
			break;
		case 0xC: // Clear variables
			m_DemoFadeTileNum = -1;
			m_moongate.m_showMoongate = MoongateStatus::CLOSED;
			m_map_demo_char.clear();
			m_playerHit = -1;
			m_demoInstructionNum++;
			break;
		case 0xD: // Move character
			m_demoInstructionNum++;
			done = true;
			m_curDelayTime = SCRIPT_TICK * 6;
			m_curInstructionDelay = 0;
			char_num = curInstruction.data[0];
			if (m_map_demo_char.contains(char_num))
			{
				// Fortunately, because they are based on bytes, they can never exceed an int value
				switch (curInstruction.data[1])
				{
				case 0: // Up
					m_map_demo_char[char_num].y--;
					break;
				case 1: // Right
					m_map_demo_char[char_num].x++;
					break;
				case 2: // Down
					m_map_demo_char[char_num].y++;
					break;
				case 3: // Left
					m_map_demo_char[char_num].x--;
					break;
				default:
					break;
				}
			}
			break;
		case 0xE: // Loop.  We could be better and have a stack to allow multiple loops, but the game doesn't do this, nor does the script have any loops in loops
			m_demoInstructionNum++;
			m_loopPos = m_demoInstructionNum;
			m_numLoops = curInstruction.data[0];
			break;
		case 0xF:
			m_numLoops--;
			if (m_numLoops <= 0)
			{
				m_demoInstructionNum++;
			}
			else
			{
				m_demoInstructionNum = m_loopPos;
			}
			break;
		default:
			done = true;
			m_curInstructionDelay = SCRIPT_TICK;
			break;
		}
	}
}
