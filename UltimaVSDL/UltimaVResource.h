#pragma once
#include <vector>
#include <cstdint>

struct U5StoryData
{
	int text_left_pos;
	int text_right_pos;
};

struct U5StoryText
{
	U5StoryText() :
		text_offset(0),
		image_index(0),
		story_number(0),
		picture_x(0),
		picture_y(0),
		action(0)
	{
	}
	size_t text_offset;
	std::vector<unsigned char> text;
	int image_index;
	int story_number;
	int picture_x;
	int picture_y;
	int action;
};

struct U5Data
{
	std::vector<U5StoryData> story_data{ 0x2a };
	std::vector<U5StoryText> story_text{ 21 };
};

enum class BitFileNames : uint_fast8_t
{
	BRITISH = 0,
	TITLE = 1,
	WD = 2
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

class UltimaVResource
{
public:
	UltimaVResource();
	~UltimaVResource();

	int LoadResources();

	std::vector<std::vector<U5ImageData>> m_BitFileData;
	std::vector<unsigned char> m_PathFileData;
	std::vector<std::vector<U5ImageData>> m_Image16FileData;
	std::vector<std::vector<std::vector<U5ImageData>>> m_CharacterSetsData;
	U5Data m_data;
private:
	
	void Cleanup();

	int LoadBitFiles();
	int LoadPathFile();
	int Load16Images();
	int LoadStory(std::vector<unsigned char> &buffer);
	int LoadDataOvl();
	int LoadCharacterSets();
	int ParseCharacterFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int width, int height);
	int ParseBitFile(std::vector<U5ImageData> &bit_file_data, std::vector<unsigned char> &data);
	int Parse16File(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte);
	uint32_t ReadInt16(std::vector<unsigned char>::iterator data, size_t &curPos);
	uint32_t ReadInt32(std::vector<unsigned char>::iterator data, size_t &curPos);
	int ReadOffsets(std::vector<unsigned char> &data, int offsetSize, int numOffsets, std::vector<size_t> &file_offsets, size_t &curPos);
	int ReadImage(std::vector<unsigned char> &data, size_t offset, int numPixelsPerByte, U5ImageData& outImage);
	void LoadStoryText(const std::vector<unsigned char>& buffer, size_t pos, std::vector<unsigned char>& text);
};

