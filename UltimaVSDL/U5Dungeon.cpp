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

void U5Dungeon::SetPos(int x, int y)
{
	if (x >= 0 && y >= 0)
	{
		GameBase::SetPos(x, y);
	}
	
	CalcCurMatrix();
}

void U5Dungeon::CalcCurMatrix()
{
	m_curMatrix[1][0] = m_parent->m_currentDungeonMap[m_xpos][m_ypos];
	switch (m_dir)
	{
	case 'N':
		m_curMatrix[0][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][m_ypos];
		m_curMatrix[2][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][m_ypos];

		m_curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		m_curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];
		m_curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];
		m_curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 2) + 8) % 8];

		m_curMatrix[0][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 3) + 8) % 8];
		m_curMatrix[1][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos - 3) + 8) % 8];
		m_curMatrix[2][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 3) + 8) % 8];
		break;
	case 'S':
		m_curMatrix[0][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][m_ypos];
		m_curMatrix[2][0] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][m_ypos];

		m_curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		m_curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];
		m_curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];
		m_curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 2) + 8) % 8];

		m_curMatrix[0][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 3) + 8) % 8];
		m_curMatrix[1][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 0) + 8) % 8][static_cast<size_t>((m_ypos + 3) + 8) % 8];
		m_curMatrix[2][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 3) + 8) % 8];
		break;
	case 'E':
		m_curMatrix[0][0] = m_parent->m_currentDungeonMap[m_xpos][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[2][0] = m_parent->m_currentDungeonMap[m_xpos][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		m_curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		m_curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 2) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];

		m_curMatrix[0][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 3) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		m_curMatrix[1][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 3) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos + 3) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		break;
	case 'W':
		m_curMatrix[0][0] = m_parent->m_currentDungeonMap[m_xpos][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[2][0] = m_parent->m_currentDungeonMap[m_xpos][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		m_curMatrix[0][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[1][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][1] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 1) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		m_curMatrix[0][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[1][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][2] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 2) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];

		m_curMatrix[0][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 3) + 8) % 8][static_cast<size_t>((m_ypos + 1) + 8) % 8];
		m_curMatrix[1][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 3) + 8) % 8][static_cast<size_t>((m_ypos + 0) + 8) % 8];
		m_curMatrix[2][3] = m_parent->m_currentDungeonMap[static_cast<size_t>((m_xpos - 3) + 8) % 8][static_cast<size_t>((m_ypos - 1) + 8) % 8];
		break;
	default:
		break;
	}
}

bool U5Dungeon::DrawFirstLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right, bool hasSign)
{
	auto& curTextures = m_sdl_helper->m_ImageDungeonTextures[static_cast<int>(m_dungeon_type)];
	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);
	bool drawsides = false;

	switch (middle.first)
	{
	case 1: // ladder up
	case 2: // ladder down
	case 3: // ladder up/down
	case 4: // chest
	case 5: // fountain
	case 6: // pit
	case 7: // open chest
		drawsides = true;
		break;
	case 11: // Should never happen, but handle incase of modding
	case 12:
	case 13:
		m_sdl_helper->RenderTextureAt(curTextures[12], hMult * 24, vMult * 22, hMult * 80, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[12], hMult * 103, vMult * 22, hMult * 80, vMult * 164, 2);
		break;
	case 14: // Passing through a door.  This will display the passageway texture without the black rectangle obscuring what's ahead
		m_sdl_helper->RenderTextureFromTo(curTextures[12], 0, 0, 24, 164, hMult * 24, vMult * 22, hMult * 24, vMult * 164);
		m_sdl_helper->RenderTextureFromTo(curTextures[12], 24, 0, 56, 28, hMult * 48, vMult * 22, hMult * 56, vMult * 28);
		m_sdl_helper->RenderTextureFromTo(curTextures[12], 24, 0, 56, 28, hMult * 103, vMult * 22, hMult * 56, vMult * 28, 2);
		m_sdl_helper->RenderTextureFromTo(curTextures[12], 0, 0, 24, 164, hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
		break;
	case 15:
		m_sdl_helper->RenderTextureAt(curTextures[12], hMult * 24, vMult * 22, hMult * 80, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[12], hMult * 103, vMult * 22, hMult * 80, vMult * 164, 2);
		break;
	default:
		drawsides = true;
		break;
	}

	if (drawsides)
	{
		switch (left.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderTextureAt(curTextures[0], hMult * 24, vMult * 22, hMult * 24, vMult * 164);
			break;
		case 12:
			m_sdl_helper->RenderTextureAt(curTextures[20], hMult * 24, vMult * 22, hMult * 24, vMult * 164);
			break;
		case 14:
			m_sdl_helper->RenderTextureAt(curTextures[4], hMult * 24, vMult * 22, hMult * 24, vMult * 164);
			break;
		default:
			m_sdl_helper->RenderTextureAt(curTextures[16], hMult * 24, vMult * 22, hMult * 24, vMult * 164);
			break;
		}

		switch (right.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderFlipTextureAt(curTextures[0], hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
			break;
		case 12:
			m_sdl_helper->RenderFlipTextureAt(curTextures[20], hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
			break;
		case 14:
			m_sdl_helper->RenderFlipTextureAt(curTextures[4], hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
			break;
		default:
			m_sdl_helper->RenderFlipTextureAt(curTextures[16], hMult * 159, vMult * 22, hMult * 24, vMult * 164, 2);
			break;
		}
	}
	// Draw the sign if it exists
	// This should fit on the wall, but the game draws it above all other graphics, excluding items and monsters
	if (hasSign)
	{
		uint8_t sign_num = m_curMatrix[1][1].second;
		if (sign_num > 0)
		{
			sign_num--;
			if (sign_num < m_parent->m_sign_data.size())
			{
				const int RENDER_OFFSET = 1;
				m_sdl_helper->DrawDungeonSign(m_parent->m_sign_data[sign_num].text, m_parent->m_sign_data[sign_num].x + RENDER_OFFSET, m_parent->m_sign_data[sign_num].y + RENDER_OFFSET);
			}
		}
	}

	// Draw items last
	switch (middle.first)
	{
	case 1:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 64, vMult * 24, hMult * 40, vMult * 80, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 103, vMult * 24, hMult * 40, vMult * 80, 3);
		break;
	case 2:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 64, vMult * 104, hMult * 40, vMult * 80, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 103, vMult * 104, hMult * 40, vMult * 80, 2);
		break;
	case 3:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 64, vMult * 24, hMult * 40, vMult * 80, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 103, vMult * 24, hMult * 40, vMult * 80, 3);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 64, vMult * 104, hMult * 40, vMult * 80, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][0], hMult * 103, vMult * 104, hMult * 40, vMult * 80, 2);
		break;
	case 4:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][12], hMult * 64, vMult * 160, hMult * 40, vMult * 24, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][12], hMult * 103, vMult * 160, hMult * 40, vMult * 24, 2);
		break;
	case 5:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][4], hMult * 64, vMult * 104, hMult * 40, vMult * 80, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][4], hMult * 103, vMult * 104, hMult * 40, vMult * 80, 2);
		break;
	case 6:
		switch (middle.second & 0b111)
		{
		case 0:
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][8], hMult * 64, vMult * 160, hMult * 40, vMult * 24, 0);
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][8], hMult * 103, vMult * 160, hMult * 40, vMult * 24, 2);
			break;
		default:
			break;
		}
		break;
	case 7:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][16], hMult * 64, vMult * 160, hMult * 40, vMult * 24, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][16], hMult * 103, vMult * 160, hMult * 40, vMult * 24, 2);
		break;
	default:
		break;
	}

	if (middle.second & 0x8) // Draw a pit above
	{
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][8], hMult * 64, vMult * 24, hMult * 40, vMult * 24, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][8], hMult * 103, vMult * 24, hMult * 40, vMult * 24, 3);
	}
	
	return true;
}

bool U5Dungeon::DrawSecondLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right, bool &hasSign)
{
	auto& curTextures = m_sdl_helper->m_ImageDungeonTextures[static_cast<int>(m_dungeon_type)];
	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);
	bool drawsides = false;
	hasSign = false;

	switch (middle.first)
	{
	case 1: // ladder up
	case 2: // ladder down
	case 3: // ladder up/down
	case 4: // chest
	case 5: // fountain
	case 6: // pit
	case 7: // open chest
		drawsides = true;
		break;
	case 11:
		m_sdl_helper->RenderTextureAt(curTextures[9], hMult * 48, vMult * 22, hMult * 56, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[9], hMult * 103, vMult * 22, hMult * 56, vMult * 164, 2);
		if (middle.second > 0)
		{
			hasSign = true;
		}
		break;
	case 12:
		m_sdl_helper->RenderTextureAt(curTextures[25], hMult * 48, vMult * 22, hMult * 56, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[25], hMult * 103, vMult * 22, hMult * 56, vMult * 164, 2);
		if (middle.second > 0)
		{
			hasSign = true;
		}
		break;
	case 15:
		m_sdl_helper->RenderTextureAt(curTextures[13], hMult * 48, vMult * 22, hMult * 56, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[13], hMult * 103, vMult * 22, hMult * 56, vMult * 164, 2);
		break;
	default:
		drawsides = true;
		break;
	};
	if (drawsides)
	{
		switch (left.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderTextureAt(curTextures[1], hMult * 48, vMult * 22, hMult * 32, vMult * 164);
			break;
		case 12:
			m_sdl_helper->RenderTextureAt(curTextures[21], hMult * 48, vMult * 22, hMult * 32, vMult * 164);
			break;
		case 14:
			m_sdl_helper->RenderTextureAt(curTextures[5], hMult * 48, vMult * 22, hMult * 32, vMult * 164);
			break;
		default:
			m_sdl_helper->RenderTextureAt(curTextures[17], hMult * 48, vMult * 22, hMult * 32, vMult * 164);
			break;
		}

		switch (right.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderFlipTextureAt(curTextures[1], hMult * 127, vMult * 22, hMult * 32, vMult * 164, 2);
			break;
		case 12:
			m_sdl_helper->RenderFlipTextureAt(curTextures[21], hMult * 127, vMult * 22, hMult * 32, vMult * 164, 2);
			break;
		case 14:
			m_sdl_helper->RenderFlipTextureAt(curTextures[5], hMult * 127, vMult * 22, hMult * 32, vMult * 164, 2);
			break;
		default:
			m_sdl_helper->RenderFlipTextureAt(curTextures[17], hMult * 127, vMult * 22, hMult * 32, vMult * 164, 2);
			break;
		}
	}

	// Draw items last
	switch (middle.first)
	{
	case 1:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 80, vMult * 48, hMult * 24, vMult * 56, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 103, vMult * 48, hMult * 24, vMult * 56, 3);
		break;
	case 2:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 80, vMult * 104, hMult * 24, vMult * 56, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 103, vMult * 104, hMult * 24, vMult * 56, 2);
		break;
	case 3:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 80, vMult * 48, hMult * 24, vMult * 56, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 103, vMult * 48, hMult * 24, vMult * 56, 3);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 80, vMult * 104, hMult * 24, vMult * 56, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][1], hMult * 103, vMult * 104, hMult * 24, vMult * 56, 2);
		break;
	case 4:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][13], hMult * 80, vMult * 128, hMult * 24, vMult * 32, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][13], hMult * 103, vMult * 128, hMult * 24, vMult * 32, 2);
		break;
	case 5:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][5], hMult * 80, vMult * 104, hMult * 24, vMult * 56, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][5], hMult * 103, vMult * 104, hMult * 24, vMult * 56, 2);
		break;
	case 6:
		switch (middle.second & 0b111)
		{
		case 0:
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][9], hMult * 80, vMult * 128, hMult * 24, vMult * 32, 0);
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][9], hMult * 103, vMult * 128, hMult * 24, vMult * 32, 2);
			break;
		default:
			break;
		}
		break;
	case 7:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][17], hMult * 80, vMult * 128, hMult * 24, vMult * 32, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][17], hMult * 103, vMult * 128, hMult * 24, vMult * 32, 2);
		break;
	default:
		break;
	}

	if (middle.second & 0x8) // Draw a pit above
	{
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][9], hMult * 80, vMult * 48, hMult * 24, vMult * 32, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][9], hMult * 103, vMult * 48, hMult * 24, vMult * 32, 3);
	}

	return true;
}

bool U5Dungeon::DrawThirdLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right)
{
	auto& curTextures = m_sdl_helper->m_ImageDungeonTextures[static_cast<int>(m_dungeon_type)];
	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);
	bool drawsides = false;

	switch (middle.first)
	{
	case 1: // ladder up
	case 2: // ladder down
	case 3: // ladder up/down
	case 4: // chest
	case 5: // fountain
	case 6: // pit
	case 7: // open chest
		drawsides = true;
		break;
	case 11:
		m_sdl_helper->RenderTextureAt(curTextures[10], hMult * 80, vMult * 22, hMult * 24, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[10], hMult * 103, vMult * 22, hMult * 24, vMult * 164, 2);
		break;
	case 12:
		m_sdl_helper->RenderTextureAt(curTextures[26], hMult * 80, vMult * 22, hMult * 24, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[26], hMult * 103, vMult * 22, hMult * 24, vMult * 164, 2);
		break;
	case 15:
		m_sdl_helper->RenderTextureAt(curTextures[14], hMult * 80, vMult * 22, hMult * 24, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[14], hMult * 103, vMult * 22, hMult * 24, vMult * 164, 2);
		break;
	default:
		drawsides = true;
		break;
	};
	if (drawsides)
	{
		switch (left.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderTextureAt(curTextures[2], hMult * 80, vMult * 22, hMult * 16, vMult * 164);
			break;
		case 12:
			m_sdl_helper->RenderTextureAt(curTextures[22], hMult * 80, vMult * 22, hMult * 16, vMult * 164);
			break;
		case 14:
			m_sdl_helper->RenderTextureAt(curTextures[6], hMult * 80, vMult * 22, hMult * 16, vMult * 164);
			break;
		default:
			m_sdl_helper->RenderTextureAt(curTextures[18], hMult * 80, vMult * 22, hMult * 16, vMult * 164);
			break;
		}

		switch (right.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderFlipTextureAt(curTextures[2], hMult * 111, vMult * 22, hMult * 16, vMult * 164, 2);
			break;
		case 12:
			m_sdl_helper->RenderFlipTextureAt(curTextures[22], hMult * 111, vMult * 22, hMult * 16, vMult * 164, 2);
			break;
		case 14:
			m_sdl_helper->RenderFlipTextureAt(curTextures[6], hMult * 111, vMult * 22, hMult * 16, vMult * 164, 2);
			break;
		default:
			m_sdl_helper->RenderFlipTextureAt(curTextures[18], hMult * 111, vMult * 22, hMult * 16, vMult * 164, 2);
			break;
		}
	}

	// Draw items last
	switch (middle.first)
	{
	case 1:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 88, vMult * 80, hMult * 16, vMult * 24, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 103, vMult * 80, hMult * 16, vMult * 24, 3);
		break;
	case 2:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 88, vMult * 104, hMult * 16, vMult * 24, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 103, vMult * 104, hMult * 16, vMult * 24, 2);
		break;
	case 3:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 88, vMult * 80, hMult * 16, vMult * 24, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 103, vMult * 80, hMult * 16, vMult * 24, 3);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 88, vMult * 104, hMult * 16, vMult * 24, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][2], hMult * 103, vMult * 104, hMult * 16, vMult * 24, 2);
		break;
	case 4:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][14], hMult * 88, vMult * 112, hMult * 16, vMult * 16, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][14], hMult * 103, vMult * 112, hMult * 16, vMult * 16, 2);
		break;
	case 5:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][6], hMult * 88, vMult * 104, hMult * 16, vMult * 24, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][6], hMult * 103, vMult * 104, hMult * 16, vMult * 24, 2);
		break;
	case 6:
		switch (middle.second & 0b111)
		{
		case 0:
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][10], hMult * 88, vMult * 112, hMult * 16, vMult * 16, 0);
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][10], hMult * 103, vMult * 112, hMult * 16, vMult * 16, 2);
			break;
		default:
			break;
		}
		break;
	case 7:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][18], hMult * 88, vMult * 112, hMult * 16, vMult * 16, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][18], hMult * 103, vMult * 112, hMult * 16, vMult * 16, 2);
		break;
	default:
		break;
	}

	if (middle.second & 0x8) // Draw a pit above
	{
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][10], hMult * 88, vMult * 80, hMult * 16, vMult * 16, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][10], hMult * 103, vMult * 80, hMult * 16, vMult * 16, 3);
	}

	return true;
}

bool U5Dungeon::DrawFourthLayer(std::pair<uint8_t, uint8_t> left, std::pair<uint8_t, uint8_t> middle, std::pair<uint8_t, uint8_t> right)
{
	auto& curTextures = m_sdl_helper->m_ImageDungeonTextures[static_cast<int>(m_dungeon_type)];
	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);
	float vMult = window_height / static_cast<float>(ORIGINAL_GAME_HEIGHT);
	float hMult = window_width / static_cast<float>(ORIGINAL_GAME_WIDTH);
	bool drawsides = false;

	switch (middle.first)
	{
	case 1: // ladder up
	case 2: // ladder down
	case 3: // ladder up/down
	case 4: // chest
	case 5: // fountain
	case 6: // pit
	case 7: // open chest
		drawsides = true;
		break;
	case 11:
		m_sdl_helper->RenderTextureAt(curTextures[11], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[11], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
		break;
	case 12:
		m_sdl_helper->RenderTextureAt(curTextures[27], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[27], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
		break;
	case 15:
		m_sdl_helper->RenderTextureAt(curTextures[15], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
		m_sdl_helper->RenderFlipTextureAt(curTextures[15], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
		//drawsides = true;
		break;
	default:
		drawsides = true;
		break;
	};
	if (drawsides)
	{
		switch (left.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderTextureAt(curTextures[3], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
			break;
		case 12:
			m_sdl_helper->RenderTextureAt(curTextures[23], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
			break;
		case 14:
			m_sdl_helper->RenderTextureAt(curTextures[7], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
			break;
		default:
			m_sdl_helper->RenderTextureAt(curTextures[19], hMult * 96, vMult * 22, hMult * 8, vMult * 164);
			break;
		}

		switch (right.first)
		{
		case 11:
		case 13:
			m_sdl_helper->RenderFlipTextureAt(curTextures[3], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
			break;
		case 12:
			m_sdl_helper->RenderFlipTextureAt(curTextures[23], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
			break;
		case 14:
			m_sdl_helper->RenderFlipTextureAt(curTextures[7], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
			break;
		default:
			m_sdl_helper->RenderFlipTextureAt(curTextures[19], hMult * 103, vMult * 22, hMult * 8, vMult * 164, 2);
			break;
		}
	}
	// Draw items last
	switch (middle.first)
	{
	case 1:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 96, vMult * 96, hMult * 8, vMult * 8, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 103, vMult * 96, hMult * 8, vMult * 8, 3);
		break;
	case 2:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 96, vMult * 104, hMult * 8, vMult * 8, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 103, vMult * 104, hMult * 8, vMult * 8, 2);
		break;
	case 3:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 96, vMult * 96, hMult * 8, vMult * 8, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 103, vMult * 96, hMult * 8, vMult * 8, 3);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 96, vMult * 104, hMult * 8, vMult * 8, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][3], hMult * 103, vMult * 104, hMult * 8, vMult * 8, 2);
		break;
	case 4:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][15], hMult * 96, vMult * 104, hMult * 8, vMult * 8, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][15], hMult * 103, vMult * 104, hMult * 8, vMult * 8, 2);
		break;
	case 5:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][7], hMult * 96, vMult * 104, hMult * 8, vMult * 8, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][7], hMult * 103, vMult * 104, hMult * 8, vMult * 8, 2);
		break;
	case 6:
		switch (middle.second & 0b111)
		{
		case 0:
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][11], hMult * 96, vMult * 104, hMult * 8, vMult * 8, 0);
			m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][11], hMult * 103, vMult * 104, hMult * 8, vMult * 8, 2);
			break;
		default:
			break;
		}
		break;
	case 7:
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][19], hMult * 88, vMult * 112, hMult * 16, vMult * 16, 0);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][19], hMult * 103, vMult * 112, hMult * 16, vMult * 16, 2);
		break;
	default:
		break;
	}

	if (middle.second & 0x8) // Draw a pit above
	{
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][11], hMult * 96, vMult * 96, hMult * 8, vMult * 8, 1);
		m_sdl_helper->RenderFlipTextureAt(m_sdl_helper->m_ImageMaskTextures[IMV_ITEMS][11], hMult * 103, vMult * 96, hMult * 8, vMult * 8, 3);
	}

	return true;
}

void U5Dungeon::DrawRoom()
{
	
	std::pair<uint8_t, uint8_t> tempval;

	int window_width, window_height;
	m_sdl_helper->GetScreenDimensions(window_width, window_height);

	int max_depth = 3;
	// Get the furthest to draw out
	for (int index = 0; index < max_depth; index++)
	{
		auto& curMiddle = m_curMatrix[1][index];
		if (curMiddle.first > 9 && curMiddle.first != 14)
		{
			max_depth = index;
			break;
		}
	}
	bool ret;
	bool hasSign = false;
	if (max_depth > 2)
	{
		ret = DrawFourthLayer(m_curMatrix[0][3], m_curMatrix[1][3], m_curMatrix[2][3]);
	}
	if (max_depth > 1)
	{
		ret = DrawThirdLayer(m_curMatrix[0][2], m_curMatrix[1][2], m_curMatrix[2][2]);
	}
	if (max_depth > 0)
	{
		ret = DrawSecondLayer(m_curMatrix[0][1], m_curMatrix[1][1], m_curMatrix[2][1], hasSign);
	}

	ret = DrawFirstLayer(m_curMatrix[0][0], m_curMatrix[1][0], m_curMatrix[2][0], hasSign);
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

	if (m_xpos >= 10)
	{
		int tempval = (m_xpos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 8, 0);
	}
	int tempval1 = (m_xpos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval1)], 9, 0);

	if (m_ypos >= 10)
	{
		int tempval = (m_ypos / 10) % 10;
		m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval)], 11, 0);
	}
	int tempval2 = (m_ypos) % 10;
	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + tempval2)], 12, 0);

	m_sdl_helper->DrawTileTexture8(m_sdl_helper->m_CharacterSetsTextures[0][0][static_cast<size_t>('0' + m_parent->m_cur_level)], 15, 0);

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
			GoForward();
			break;
		case SDLK_DOWN:
			GoBackward();
			break;
		case SDLK_LEFT:
			TurnLeft();
			break;
		case SDLK_RIGHT:
			TurnRight();
			break;
		case SDLK_F24:
			TurnAround();
			break;
		case SDLK_E:
			//ProcessEnter();
			break;
		case SDLK_K:
			Klimb();
			break;
		default:
			m_input->m_isValid = false;
			break;
		}
	}
}

int U5Dungeon::checkValidLocation(const std::pair<int, int>& pos_info)
{
	return 0;
}

int U5Dungeon::ProcessUpDown()
{
	m_input->m_isValid = true;
	SDL_Keycode curKey = m_input->GetKeyCode();
	switch (curKey)
	{
	case SDLK_UP:
	case SDLK_U:
		return 'U';
	case SDLK_DOWN:
	case SDLK_D:
		return 'D';
	default:
		m_input->m_isValid = false;
		break;
	}
	return -1;
}

void U5Dungeon::HandleKlimb()
{
	int ret = ProcessUpDown();
	m_input->SetRequireAllKeysUp();
	if (ret == 'U')
	{
		m_process_key = std::bind(&U5Dungeon::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_17[UP_STRING]);
		m_parent->ChangeLevel(-1);
	}
	else if (ret == 'D')
	{
		m_process_key = std::bind(&U5Dungeon::ProcessAnyKeyHit, this);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_17[DOWN_STRING]);
		m_parent->ChangeLevel(1);
	}
}

void U5Dungeon::Klimb()
{
	uint8_t curTile = m_curMatrix[1][0].first;
	m_input->SetRequireAllKeysUp();
	switch (curTile)
	{
	case 1: // Ladder up
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_4[KLIMB_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_17[UP_STRING]);
		m_parent->ChangeLevel(-1);
		break;
	case 2: // Ladder down
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_4[KLIMB_STRING]);
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_17[DOWN_STRING]);
		m_parent->ChangeLevel(1);
		break;
	case 3: // Prompt up/down
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_4[KLIMB_PROMPT]);
		m_process_key = std::bind(&U5Dungeon::HandleKlimb, this);
		break;
	default:
		m_parent->m_console->PrintText(m_resources->m_data.game_strings_4[KLIMB_WHAT]);
		m_parent->m_console->NewPrompt();
		break;
	}
}

void U5Dungeon::GoForward()
{
	std::pair<int, int> tempos = { m_xpos, m_ypos };
	switch (m_dir)
	{
	case 'N':
		tempos.second--;
		tempos.second += 8;
		tempos.second %= 8;
		break;
	case 'S':
		tempos.second++;
		tempos.second %= 8;
		break;
	case 'E':
		tempos.first++;
		tempos.first %= 8;
		break;
	case 'W':
		tempos.first--;
		tempos.first += 8;
		tempos.first %= 8;
		break;
	default:
		break;
	}
	if (0 != checkValidLocation(tempos))
	{
		return;
	}
	SetPos(tempos.first, tempos.second);
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_3[DUNGEON_ADVANCE]);
	m_parent->m_console->NewPrompt();
}

void U5Dungeon::GoBackward()
{
	std::pair<int, int> tempos = { m_xpos, m_ypos };
	switch (m_dir)
	{
	case 'S':
		tempos.second--;
		tempos.second += 8;
		tempos.second %= 8;
		break;
	case 'N':
		tempos.second++;
		tempos.second %= 8;
		break;
	case 'W':
		tempos.first++;
		tempos.first %= 8;
		break;
	case 'E':
		tempos.first--;
		tempos.first += 8;
		tempos.first %= 8;
		break;
	default:
		break;
	}
	if (0 != checkValidLocation(tempos))
	{
		return;
	}
	SetPos(tempos.first, tempos.second);
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_3[DUNGEON_BACKUP]);
	m_parent->m_console->NewPrompt();
}

void U5Dungeon::TurnAround()
{
	switch (m_dir)
	{
	case 'N':
		m_dir = 'S';
		break;
	case 'S':
		m_dir = 'N';
		break;
	case 'E':
		m_dir = 'W';
		break;
	case 'W':
		m_dir = 'E';
		break;
	default:
		break;
	}
	CalcCurMatrix();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_3[DUNGEON_TURN_AROUND]);
	m_parent->m_console->NewPrompt();
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
	CalcCurMatrix();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_3[DUNGEON_TURN_LEFT]);
	m_parent->m_console->NewPrompt();
}

void U5Dungeon::TurnRight()
{
	// TO DO: Check if valid location
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
	CalcCurMatrix();
	m_parent->m_console->PrintText(m_resources->m_data.game_strings_3[DUNGEON_TURN_RIGHT]);
	m_parent->m_console->NewPrompt();
}
