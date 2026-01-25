#pragma once

const int ORIGINAL_GAME_WIDTH = 320;
const int ORIGINAL_GAME_HEIGHT = 200;

const int RENDER_WIDTH = 1280;
const int RENDER_HEIGHT = 800;

const int LINE_THICKNESS = 4;

enum class U5Modes
{
	None,
	SplashScreen,
	Menu,
	MenuSkip,
	Game,
	Cutscene
};

const int RENDER_TILE_HEIGHT = 64;
const int RENDER_TILE_WIDTH = 64;
const int HALF_TILE_HEIGHT = 32;
const int HALF_TILE_WIDTH = 32;

enum IMAGE_16_VAL
{
	IV16_CREATE,
	IV16_END1,
	IV16_END2,
	IV16_ENDSC,
	IV16_STARTSC,
	IV16_STORY1,
	IV16_STORY2,
	IV16_STORY3,
	IV16_STORY4,
	IV16_STORY5,
	IV16_STORY6,
	IV16_TEXT,
	IV16_ULTIMA
};