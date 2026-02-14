#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <array>
#include <utility>
#include "U5Enums.h"
#include <SDL3/SDL_stdinc.h>

struct NPC_Schedule
{
	uint8_t AI_types[3];
	uint8_t x_coordinates[3];
	uint8_t y_coordinates[3];
	int8_t z_coordinates[3];
	uint8_t times[4];
};

struct NPC_Data
{
	NPC_Schedule schedule;
	uint8_t facing_direction;
	uint8_t current_x;
	uint8_t current_y;
	uint8_t current_z;
	uint8_t type;
	uint8_t dialog_number;
	Uint64 curNPCDelay;
};

struct NPC_Info
{
	NPC_Data data[32];
};

struct NPC_File
{
	NPC_Info info[8]; // each NPC file has information for 8 maps
};

struct U5StoryParagraphInfo
{
	U5StoryParagraphInfo() :
		text_left_pos(0),
		text_right_pos(0),
		y_extent(0)
	{
	}
	int text_left_pos;
	int text_right_pos;
	int y_extent;
};

struct U5StoryScreen
{
	U5StoryScreen() :
		text_offset(0),
		image_index(0),
		story_number(0),
		picture_x(0),
		picture_y(0),
		action(0),
		first_line_offset(0),
		text_y_pos(0)
	{
		std::memset(paragraph, 0, sizeof(U5StoryParagraphInfo));
	}
	size_t text_offset;
	std::vector<unsigned char> text;
	int image_index;
	int story_number;
	int picture_x;
	int picture_y;
	int action;
	int first_line_offset;
	int text_y_pos;
	U5StoryParagraphInfo paragraph[2];
};

struct U5Data
{
	U5Data() :
		map_chunks{},
		world_map{},
		underworld_map{},
		location_z_index{},
		npc_info{}
	{
	}
	std::vector<unsigned char> buffer;
	const size_t NUM_STORIES = 21;
	const size_t NUM_ENDING = 6;
	std::vector<U5StoryScreen> story_text{ NUM_STORIES };
	std::vector<U5StoryScreen> ending_text{ NUM_ENDING };
	std::vector<unsigned char> intro_string_1;
	std::vector<unsigned char> intro_string_2;
	std::vector<std::string> intro_demo_string;
	std::vector<std::string> game_strings;
	std::vector<std::string> dungeon_sign_strings;
	std::vector<std::string> intro_strings;
	std::array<unsigned char, 0x100> map_chunks;
	std::vector<std::vector<unsigned char>> world_map;
	std::vector<std::vector<unsigned char>> underworld_map;
	std::vector<std::vector<std::vector<unsigned char>>> town_maps;
	std::vector<std::vector<std::vector<unsigned char>>> keep_maps;
	std::vector<std::vector<std::vector<unsigned char>>> dwelling_maps;
	std::vector<std::vector<std::vector<unsigned char>>> castle_maps;
	std::vector<std::vector<std::vector<std::pair<uint8_t, uint8_t>>>> dungeon_maps;
	std::array<std::pair<int, int>, 0x28> location_info;
	std::array<std::string, 0x28> location_names;
	std::array<int, 0x20> location_z_index;
	std::array< NPC_File, 4> npc_info;
	std::vector<std::string> compressed_words;
};

struct IntroScriptInstruction {
	IntroScriptInstruction() :
		instruction(0),
		data{ 0,0,0,0 }
	{
	}
	unsigned char instruction;
	unsigned char data[4];
};

enum class BitFileNames : uint_fast8_t
{
	BRITISH = 0,
	TITLE = 1,
	WD = 2
};

enum class RenderMode : uint_fast8_t
{
	EGA,
	CGA
};

struct U5ImageData
{
	U5ImageData() :
		width(0),
		height(0),
		mode(0)
	{
	}
	uint32_t width;
	uint32_t height;
	int mode;

	std::vector<unsigned char> pixel_data;
};

struct U5SignData
{
	U5SignData(int curX, int curY, int curZ, std::string curText) :
		x(curX),
		y(curY),
		z(curZ),
		text(curText)
	{
	}
	int x;
	int y;
	int z;
	std::string text;
};

struct U5PaddedImageData : public U5ImageData
{
	U5PaddedImageData() :
		real_width(0),
		real_height(0)
	{
	}
	uint32_t real_width;
	uint32_t real_height;
};

const size_t NUM_CUTSCENES = 4;
const size_t NUM_DEMOS = 4;
const size_t CUTSCENE_WIDTH = 11;
const size_t CUTSCENE_HEIGHT = 11;
const size_t DEMO_WIDTH = 19;
const size_t DEMO_HEIGHT = 4;


enum TLK_ENUM
{
	TLK_STRING
};

struct U5Dialog
{
	std::vector<std::pair<int, std::string>> name;
	std::vector<std::pair<int, std::string>> description;
	std::vector<std::pair<int, std::string>> greeting;
	std::vector<std::pair<int, std::string>> job;
	std::vector<std::pair<int, std::string>> bye;
};

class UltimaVResource
{
public:
	UltimaVResource();
	~UltimaVResource();

	int LoadResources();

	std::vector<std::vector<U5ImageData>> m_BitFileData;
	std::vector<unsigned char> m_PathFileData;
	std::vector<std::vector<U5ImageData>> m_Image16FileData;
	std::vector<std::vector<U5ImageData>> m_ImageDungeonFileData;
	std::vector<std::vector<U5ImageData>> m_ImageMaskFileData;

	std::vector<std::vector<std::vector<U5ImageData>>> m_CharacterSetsData;
	std::vector<U5PaddedImageData> m_ProportionalFontData;
	std::vector<U5ImageData> m_Tiles;
	std::vector<IntroScriptInstruction> m_IntroInstructions;
	std::vector<std::vector< U5SignData>> m_SignData;
	std::vector<std::vector< U5SignData>> m_DungeonSignData;
	std::vector<std::string> m_LookData;
	U5Data m_data;
	RenderMode m_render_mode;
	int m_CutsceneMap[NUM_CUTSCENES][CUTSCENE_WIDTH][CUTSCENE_HEIGHT];
	int m_DemoMap[NUM_DEMOS][DEMO_WIDTH][DEMO_HEIGHT];
private:
	
	void Cleanup();

	int LoadBuffer(const std::string strFile, std::vector<unsigned char>& buffer);
	int LoadBitFiles();
	int LoadPathFile();
	int LoadNPCs(MapTypes map_type);
	int Load16Images();
	int LoadDungeonImages();
	int LoadMaskedImages();
	int LoadStory(std::vector<unsigned char> &buffer);
	int LoadEnding(std::vector<unsigned char>& buffer);
	int LoadDataOvl();
	int LoadCharacterSets();
	int LoadProportionalFont();
	int LoadMiscMaps();
	int LoadTiles();
	int LoadDungeons();
	int LoadWorldMap();
	int LoadUnderworldMap();
	int LoadMap(MapTypes map_type);
	int LoadMapChunk(unsigned char cur_chunk_val, size_t curChunkX, size_t curChunkY, std::vector<std::vector<unsigned char>>& map, const std::vector<unsigned char>& buffer);
	int LoadSigns();
	int LoadDungeonSigns(const std::vector<unsigned char>& buffer);
	int LoadLookData();
	int LoadTalk(MapTypes map_type);

	int ParseCharacterFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int width, int height);
	int ParseBitFile(std::vector<U5ImageData> &bit_file_data, std::vector<unsigned char> &data);
	int Parse16File(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte);
	int ParseMaskFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte);
	int MergeMask(U5ImageData& outImage, U5ImageData& mask);
	uint32_t ReadInt16(std::vector<unsigned char>::const_iterator data, size_t &curPos);
	uint32_t ReadInt32(std::vector<unsigned char>::const_iterator data, size_t &curPos);
	int ReadOffsets(const std::vector<unsigned char> &data, int offsetSize, int numOffsets, std::vector<size_t> &file_offsets, size_t &curPos);
	int ReadImage(std::vector<unsigned char> &data, size_t offset, int numPixelsPerByte, U5ImageData& outImage);
	void LoadStoryText(const std::vector<unsigned char>& buffer, size_t pos, std::vector<unsigned char>& text);
	bool ReadStrings(const std::vector<unsigned char>& buffer, std::vector<std::string>& str_vec, size_t start_pos, size_t end_pos);
	int ReadCompressedWords(const std::vector<unsigned char>& buffer);
	std::string ReadNextString(std::vector<unsigned char>::const_iterator data, std::vector<unsigned char>::const_iterator end);
	void SwapCharset(std::string& curString);
};

