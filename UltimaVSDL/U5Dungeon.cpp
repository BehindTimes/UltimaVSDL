#include "U5Dungeon.h"
#include "GameObject.h"
#include "SDL3Helper.h"
#include "UltimaVResource.h"
#include "GameBase.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include "U5Enums.h"
#include "ColorData.h"
#include <cstring>
#include "U5Game.h"
#include <utility>
#include "GameOptions.h"
#include "U5Utils.h"
#include "U5Input.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <SDL3/SDL_keycode.h>

extern std::unique_ptr<U5Utils> m_utilities;
extern std::unique_ptr<U5Input> m_input;
extern std::unique_ptr<GameOptions> m_options;

U5Dungeon::U5Dungeon(SDL3Helper* sdl_helper, UltimaVResource* u5_resources) :
	GameBase(sdl_helper, u5_resources),
	m_dungeon_type(DungeonType::CAVE),
	m_parent(nullptr)
{
	m_process_key = std::bind(&U5Dungeon::ProcessAnyKeyHit, this);
}

U5Dungeon::~U5Dungeon()
{
}

void U5Dungeon::SetParent(U5Game* parent)
{
	m_parent = parent;
}

void U5Dungeon::DrawRoom()
{
	std::pair<uint8_t, uint8_t> curMatrix[3][3];
	auto& curTextures = m_sdl_helper->m_ImageDungeonTextures[static_cast<int>(m_dungeon_type)];
	curMatrix[1][0] = m_parent->m_currentDungeonMap[m_xpos][m_ypos];
	switch (m_dir)
	{
	case 'N':
		curMatrix[0][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][m_ypos];
		curMatrix[2][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][m_ypos];

		curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];
		curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];
		curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];
		break;
	case 'S':
		curMatrix[0][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][m_ypos];
		curMatrix[2][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][m_ypos];

		curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];
		curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];
		curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];
		break;
	case 'E':
		curMatrix[0][0] = m_parent->m_currentDungeonMap[m_xpos][((m_ypos - 1) + 8) % 8];
		curMatrix[2][0] = m_parent->m_currentDungeonMap[m_xpos][((m_ypos + 1) + 8) % 8];

		curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		break;
	case 'W':
		curMatrix[0][0] = m_parent->m_currentDungeonMap[m_xpos][((m_ypos + 1) + 8) % 8];
		curMatrix[2][0] = m_parent->m_currentDungeonMap[m_xpos][((m_ypos - 1) + 8) % 8];

		curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		break;
	default:
		break;
	}

	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);

	// Draw left
	auto& curLeft = curMatrix[0][0];
	switch (curLeft.first)
	{
	case 11: // Wall
		m_sdl_helper->RenderTextureAt(curTextures[0], hMult * 24, vMult * 22, hMult * 24, vMult * 164);
		break;
	}
	auto& curMiddle = curMatrix[1][1];
	switch (curMiddle.first)
	{
	case 11: // Wall
		m_sdl_helper->RenderTextureAt(curTextures[9], hMult * 48, vMult * 22, hMult * 56, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[9], hMult * 103, vMult * 22, hMult * 56, vMult * 164, 2);
		break;
	}
	// Draw Right
	auto& curRight = curMatrix[2][0];
	switch (curRight.first)
	{
	case 11: // Wall
		m_sdl_helper->RenderFlipTextureAt(curTextures[0], hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
		break;
	}
}

void U5Dungeon::Render()
{
	DrawBorder();

	m_sdl_helper->SetRenderTarget(m_sdl_helper->m_TargetTextures[TTV_MAIN_DISPLAY_BUFFER]);
	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0, 0, 0, 0xFF);
	m_sdl_helper->ClearScreen();
	DrawRoom();
}

void U5Dungeon::ProcessEvents()
{
	if (m_process_key)
	{
		m_process_key();
	}
}

void U5Dungeon::DrawBorder()
{
	SDL_FRect line;
	unsigned char color_array[3];

	if (m_resources->m_render_mode == RenderMode::CGA)
	{
		std::memcpy(color_array, cga_table[1], sizeof(color_array));
	}
	else
	{
		std::memcpy(color_array, ega_table[1], sizeof(color_array));
	}

	m_sdl_helper->DrawTileRect(1, 0, 6, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(1, 23, 5, 1, color_array[0], color_array[1], color_array[2]);

	m_sdl_helper->DrawTileRect(17, 0, 22, 1, color_array[0], color_array[1], color_array[2]);
	m_sdl_helper->DrawTileRect(18, 23, 5, 1, color_array[0], color_array[1], color_array[2]);

	SDL_SetRenderDrawColor(m_sdl_helper->m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	line = { HALF_TILE_WIDTH - LINE_THICKNESS, HALF_TILE_HEIGHT - LINE_THICKNESS, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH * 17, HALF_TILE_HEIGHT - LINE_THICKNESS, (6 * HALF_TILE_WIDTH) + (LINE_THICKNESS), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	line = { HALF_TILE_WIDTH, HALF_TILE_HEIGHT * 23, (5 * HALF_TILE_WIDTH), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);
	line = { HALF_TILE_WIDTH * 18, HALF_TILE_HEIGHT * 23, (5 * HALF_TILE_WIDTH), LINE_THICKNESS };
	SDL_RenderFillRect(m_sdl_helper->m_renderer, &line);

	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 16, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[0], 17, 23);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 7, 0);
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_ArrowTextures[1], 6, 23);

	GameBase::DrawBorder();

	if (m_xpos >= 100)
	{
		int tempval = (m_xpos / 100) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 8, 0);
	}
	if (m_xpos >= 10)
	{
		int tempval = (m_xpos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 9, 0);
	}
	int tempval1 = (m_xpos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval1)], 10, 0);

	if (m_ypos >= 100)
	{
		int tempval = (m_ypos / 100) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 12, 0);
	}
	if (m_ypos >= 10)
	{
		int tempval = (m_ypos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 13, 0);
	}
	int tempval2 = (m_ypos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval2)], 14, 0);

	m_sdl_helper->DrawTiledText(m_resources->m_data.game_strings_3[DIR_STRING], 7, 23);
	int dir_val;
	switch (m_dir)
	{
	case 'S':
		dir_val = DIR_SOUTH_STRING;
		break;
	case 'E':
		dir_val = DIR_EAST_STRING;
		break;
	case 'W':
		dir_val = DIR_WEST_STRING;
		break;
	default:
		dir_val = DIR_NORTH_STRING;
		break;
	}
	m_sdl_helper->DrawTiledText(m_resources->m_data.game_strings_3[dir_val], 12, 23);
}

void U5Dungeon::LoadDungeonType(DungeonType type)
{
	m_dungeon_type = type;
}

void U5Dungeon::ProcessAnyKeyHit()
{
	if (m_input->isAnyKeyHit())
	{
		m_input->m_isValid = true;

		SDL_Keycode curKey = m_input->GetKeyCode();
		switch (curKey)
		{
		case SDLK_UP:
			//GoForward();
			break;
		case SDLK_DOWN:
			//GoBackward();
			break;
		case SDLK_LEFT:
			TurnLeft();
			break;
		case SDLK_RIGHT:
			TurnRight();
			break;
		case SDLK_E:
			//ProcessEnter();
			break;
		case SDLK_K:
			//ProcessKlimb();
			break;
		default:
			m_input->m_isValid = false;
			break;
		}
	}
}

void U5Dungeon::TurnLeft()
{
	switch (m_dir)
	{
	case 'N':
		m_dir = 'W';
		break;
	case 'S':
		m_dir = 'E';
		break;
	case 'E':
		m_dir = 'N';
		break;
	case 'W':
		m_dir = 'S';
		break;
	default:
		break;
	}
}

void U5Dungeon::TurnRight()
{
	switch (m_dir)
	{
	case 'N':
		m_dir = 'E';
		break;
	case 'S':
		m_dir = 'W';
		break;
	case 'E':
		m_dir = 'S';
		break;
	case 'W':
		m_dir = 'N';
		break;
	default:
		break;
	}
}
