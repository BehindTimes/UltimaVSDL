#pragma once

const int ORIGINAL_GAME_WIDTH = 320;
const int ORIGINAL_GAME_HEIGHT = 200;

const int RENDER_WIDTH = 1280;
const int RENDER_HEIGHT = 800;

const int LINE_THICKNESS = 4;

const int ORIGINAL_TILE_WIDTH = 16;
const int ORIGINAL_TILE_HEIGHT = 16;

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

const int WATERFALL_ANIMATE = 200;
const int CLOCK_ANIMATE = 200;
const int WATER_ANIMATE = 2000;
const int FIRE_ANIMATE = 100;
const int NPC_ANIMATE = 250;

const int NUM_CONSOLE_WIDTH = 16;

enum class GameLocation
{
	Dungeon,
	World,
	Underworld,
	Town
};

enum class MapTypes
{
	Town = 0,
	Dwelling,
	Castle,
	Keep,
	Dungeon
};

const int NORTH_STRING = 5;
const int SOUTH_STRING = 6;
const int EAST_STRING = 7;
const int WEST_STRING = 8;

const int DIR_NORTH_STRING = 0;
const int DIR_SOUTH_STRING = 2;
const int DIR_EAST_STRING = 1;
const int DIR_WEST_STRING = 3;
const int DIR_STRING = 6;

const int WHAT_STRING = 23;

const int KEEP_STRING = 0x23;
const int VILLAGE_STRING = 0x24;
const int TOWN_STRING = 0x25;
const int HUT_STRING = 0x21;
const int DUNGEON_STRING = 0x29;
const int DWELLING_STRING = 0x2b;
const int BLACKTHORN_PALANCE_STRING = 0x2c;
const int BRITISH_CASTLE_STRING = 0x2d;

const int YES_STRING = 10;
const int UNDERWORLD_STRING = 11;
const int BRITANNIA_STRING = 12;
const int NO_STRING = 13;
const int UP_STRING = 201;
const int DOWN_STRING = 202;
const int WITH_WHAT_STRING = 222;
const int KLIMB_STRING = 34;
const int LEAVE_TOWN_STRING = 9;
const int ENTER_STRING = 31;

const int BED_HEAD = 171;
const int BED_SLEEPING = 282;
const int STAIRS_UP = 196;
const int STAIRS_LEFT = 197;
const int STAIRS_DOWN = 198;
const int STAIRS_RIGHT = 199;
const int LADDER_UP_TILE = 200;
const int LADDER_DOWN_TILE = 201;

