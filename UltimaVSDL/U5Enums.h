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

enum IMAGE_MASK_VAL
{
	IMV_ITEMS,
	IMV_MON0,
	IMV_MON1,
	IMV_MON2,
	IMV_MON3,
	IMV_MON4,
	IMV_MON5,
	IMV_MON6,
	IMV_MON7
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

const int MAX_YELL = 30;
const int MAX_TALK = 15;

const int NORTH_STRING = 0x249;
const int SOUTH_STRING = 0x24a;
const int EAST_STRING = 0x24b;
const int WEST_STRING = 0x24c;

const int DIR_NORTH_STRING = 0x299;
const int DIR_SOUTH_STRING = 0x29b;
const int DIR_EAST_STRING = 0x29a;
const int DIR_WEST_STRING = 0x29c;
const int DIR_STRING = 0x29f;

const int DUNGEON_TURN_RIGHT = 0x2a4;
const int DUNGEON_TURN_LEFT = 0x2a6;
const int DUNGEON_ADVANCE = 0x2a2;
const int DUNGEON_BACKUP = 0x2a5;
const int DUNGEON_TURN_AROUND = 0x2a7;

const int WHAT_STRING = 0x25b;

const int KEEP_STRING = 0x288;
const int VILLAGE_STRING = 0x289;
const int TOWN_STRING = 0x28a;
const int HUT_STRING = 0x286;
const int DUNGEON_STRING = 0x28e;
const int LIGHTHOUSE_STRING = 0x290;
const int BLACKTHORN_PALANCE_STRING = 0x291;
const int BRITISH_CASTLE_STRING = 0x292;

const int YES_EXIT_STRING = 0x24e;
const int UNDERWORLD_STRING = 0x2cf;
const int BRITANNIA_STRING = 0x250;
const int NO_STRING = 0x251;
const int UP_STRING = 0x244;
const int DOWN_STRING = 0x245;
const int WITH_WHAT_STRING = 0x4e1;
const int EXIT_TO_STRING = 0x2ce;
const int KLIMB_PROMPT = 0x2d2;
const int KLIMB_STRING = 0x259;
const int KLIMB_WHAT = 0x2d5;
const int LEAVE_TOWN_STRING = 0x24d;
const int ENTER_STRING = 0x284;
const int ENTER_WHAT = 0x5d6;
const int LOOK_STRING = 0x2ff;
const int HYPHEN_STRING = 0x415;
const int THOU_DOST_SEE_STRING = 0x358;
const int YOU_SEE_STRING = 0x505;

const int YELL_STRING = 0x2f5;
const int WHAT_COLON_STRING = 0x609;

const int NO_EFFECT_STRING = 0x5fa;
const int NOTHING_STRING = 0x329;
const int TALK_STRING = 0x5e1;
const int FUNNY_NO_RESPONSE_STRING = 0x5e2;
const int GUARD_NO_RESPONSE_STRING = 0x4ee;
const int NO_RESPONSE_STRING = 0x4ef;
const int PASS_STRING = 0x2f7;
const int NOBODY_HERE = 0x4f3;
const int I_AM_CALLED = 0x506;
const int YOUR_INTEREST = 0x4fc;
const int BYE_STRING = 0x4fd;
const int END_QUOTED_STRING = 0x517;
const int MY_NAME_IS_STING = 0x4fa;
const int WITH_LANGUAGE_LIKE_THAT_STRING = 0x4fb;
const int YOU_RESPOND_STRING = 0x4ff;
const int WHAT_DID_YOU_SAY_STRING = 0x500;

const int INPUT_NAME = 0;
const int INPUT_JOB = 1;
const int INPUT_WORK = 2;
const int INPUT_BYE = 3;
const int INPUT_THANK = 4;
const size_t INPUT_NAUGHTY = 5;

const int BED_HEAD = 171;
const int BED_SLEEPING = 282;
const int STAIRS_UP = 196;
const int STAIRS_LEFT = 197;
const int STAIRS_DOWN = 198;
const int STAIRS_RIGHT = 199;
const int LADDER_UP_TILE = 200;
const int LADDER_DOWN_TILE = 201;

