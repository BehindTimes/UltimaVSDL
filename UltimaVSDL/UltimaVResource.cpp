#include <filesystem>
#include <fstream>

#include "UltimaVResource.h"
#include "LzwDecompressor.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <array>
#include "U5Enums.h"
#include "GameOptions.h"
#include <memory>
#include <iostream>

extern std::unique_ptr<GameOptions> m_options;

UltimaVResource::UltimaVResource() :
	m_render_mode(m_options->m_render_mode)
{
	std::memset(m_CutsceneMap, 0, sizeof(m_CutsceneMap));
	std::memset(m_DemoMap, 0, sizeof(m_DemoMap));
}

UltimaVResource::~UltimaVResource()
{
	Cleanup();
}

void UltimaVResource::Cleanup()
{
}

int UltimaVResource::LoadResources()
{
	// This is crucial to load first, as it has information other files will need
	if (0 != LoadDataOvl())
	{
		return -1;
	}
	if (0 != LoadTiles())
	{
		return -1;
	}
	if (0 != LoadBitFiles())
	{
		return -1;
	}
	if (0 != LoadPathFile())
	{
		return -1;
	}
	if (0 != Load16Images())
	{
		return -1;
	}
	if (0 != LoadDungeonImages())
	{
		return -1;
	}
	if (0 != LoadMaskedImages())
	{
		return -1;
	}
	if (0 != LoadCharacterSets())
	{
		return -1;
	}
	if (0 != LoadProportionalFont())
	{
		return -1;
	}
	if (0 != LoadMiscMaps())
	{
		return -1;
	}
	if (0 != LoadWorldMap())
	{
		return -1;
	}
	if (0 != LoadUnderworldMap())
	{
		return -1;
	}
	if (0 != LoadMap(MapTypes::Castle))
	{
		return -1;
	}
	if (0 != LoadMap(MapTypes::Dwelling))
	{
		return -1;
	}
	if (0 != LoadMap(MapTypes::Keep))
	{
		return -1;
	}
	if (0 != LoadMap(MapTypes::Town))
	{
		return -1;
	}
	if (0 != LoadNPCs(MapTypes::Castle))
	{
		return -1;
	}
	if (0 != LoadNPCs(MapTypes::Dwelling))
	{
		return -1;
	}
	if (0 != LoadNPCs(MapTypes::Keep))
	{
		return -1;
	}
	if (0 != LoadNPCs(MapTypes::Town))
	{
		return -1;
	}
	if (0 != LoadDungeons())
	{
		return -1;
	}
	if (0 != LoadSigns())
	{
		return -1;
	}

	return 0;
}

uint32_t UltimaVResource::ReadInt16(std::vector<unsigned char>::const_iterator data, size_t& curPos)
{
	uint32_t retVal = (static_cast<uint32_t>(data[curPos + 1]) << 8) + static_cast<uint32_t>(data[curPos]);
	curPos += 2;
	return retVal;
}

uint32_t UltimaVResource::ReadInt32(std::vector<unsigned char>::const_iterator data, size_t& curPos)
{
	uint32_t retVal = (static_cast<uint32_t>(data[curPos + 3]) << 24) + (static_cast<uint32_t>(data[curPos + 2]) << 16) +
		(static_cast<uint32_t>(data[curPos + 1]) << 8) + static_cast<uint32_t>(data[curPos]);
	curPos += 4;
	return retVal;
}

int UltimaVResource::ReadOffsets(std::vector<unsigned char> &data, int offsetSize, int numOffsets, std::vector<size_t> &file_offsets, size_t &curPos)
{
	if (data.size() < curPos + static_cast<size_t>(numOffsets * offsetSize))
	{
		return -1;
	}
	file_offsets.resize(numOffsets);
	for (int index = 0; index < numOffsets; index++)
	{
		uint32_t offset = 0;
		if (offsetSize == 2)
		{
			offset = ReadInt16(data.begin(), curPos);
		}
		else if (offsetSize == 4)
		{
			offset = ReadInt32(data.begin(), curPos);
		}
		else
		{
			return -2;
		}
		file_offsets[index] = offset;
	}
	return 0;
}

int UltimaVResource::ReadImage(std::vector<unsigned char>& data, size_t offset, int numPixelsPerByte, U5ImageData &outImage)
{
	if (data.size() < offset + 4)
	{
		return -1;
	}
	outImage.width = ReadInt16(data.begin(), offset);
	outImage.height = ReadInt16(data.begin(), offset);

	uint32_t bufWidth = 0;

	int bufNum = (8 / numPixelsPerByte) * 2;
	bufWidth = (bufNum - (outImage.width % bufNum)) % bufNum;

	int modnum = 0;
	int bitInc = 0;
	if (numPixelsPerByte == 8)
	{
		modnum = 0b1;
		bitInc = 1;
	}
	else if (numPixelsPerByte == 4)
	{
		modnum = 0b11;
		bitInc = 2;
	}
	else if (numPixelsPerByte == 2)
	{
		modnum = 0b1111;
		bitInc = 4;
	}
	else
	{
		return -2;
	}

	outImage.pixel_data.resize(static_cast<size_t>(outImage.width * outImage.height));
	outImage.mode = numPixelsPerByte;

	int data_size = (outImage.width + bufWidth) * outImage.height / numPixelsPerByte;

	// Exceeds the size of the buffer
	if (data.size() < offset + data_size)
	{
		return -3;
	}
	size_t curPos = offset;
	for (uint32_t indexY = 0; indexY < outImage.height; indexY++)
	{
		for (uint32_t indexX = 0; indexX < data_size / outImage.height; indexX++)
		{
			unsigned char curByte = data[curPos];
			for (int byteIndex = 0; byteIndex < 8; byteIndex += bitInc)
			{
				int curColor = (static_cast<int>(curByte) >> ((8 - bitInc) - byteIndex)) & modnum;
				if (indexX * numPixelsPerByte + (byteIndex / bitInc) < outImage.width)
				{
					outImage.pixel_data[static_cast<size_t>(indexY * outImage.width + indexX * numPixelsPerByte + (byteIndex / bitInc))] = static_cast<unsigned char>(curColor);
				}
			}
			curPos++;
		}
	}
	return 0;
}

int UltimaVResource::ParseMaskFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte)
{
	if (data.size() < 2)
	{
		return -1;
	}
	size_t curPos = 0;
	uint32_t numImages = ReadInt16(data.begin(), curPos);
	if (numImages == 0 || numImages > 32)
	{
		return -2;
	}
	bit_file_data.resize(numImages);
	std::vector<size_t> file_offsets;
	if (0 != ReadOffsets(data, 2, numImages * 2, file_offsets, curPos))
	{
		return -3;
	}
	for (uint32_t index = 0; index < numImages; index++)
	{
		if (file_offsets[index] == 0)
		{
			return -4; // Only dungeons have missing offsets
		}
		if (0 != ReadImage(data, file_offsets[static_cast<size_t>(index * 2)], numPixelsPerByte, bit_file_data[index]))
		{
			return -5; // Invalid data
		}
		U5ImageData mask_data;
		if (0 != ReadImage(data, file_offsets[static_cast<size_t>(index * 2) + 1], 8, mask_data))
		{
			return -5; // Invalid data
		}
		if (0 != MergeMask(bit_file_data[index], mask_data))
		{
			return -6; // Invalid mask
		}
	}
	return 0;
}

int UltimaVResource::MergeMask(U5ImageData& outImage, U5ImageData& mask)
{
	if (outImage.pixel_data.size() != mask.pixel_data.size())
	{
		return -1;
	}
	for (size_t index = 0; index < outImage.pixel_data.size(); index++)
	{
		if (mask.pixel_data[index] == 1)
		{
			outImage.pixel_data[index] |= 0x80;
		}
	}
	return 0;
}

int UltimaVResource::Parse16File(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte)
{
	if (data.size() < 2)
	{
		return -1;
	}
	size_t curPos = 0;
	uint32_t numImages = ReadInt16(data.begin(), curPos);
	if (numImages == 0 || numImages > 32)
	{
		return -2;
	}
	bit_file_data.resize(numImages);
	std::vector<size_t> file_offsets;
	if (0 != ReadOffsets(data, 4, numImages, file_offsets, curPos))
	{
		return -3;
	}
	for (uint32_t index = 0; index < numImages; index++)
	{
		if (file_offsets[index] == 0)
		{
			continue; // Only dungeons have missing offsets
		}
		if (0 != ReadImage(data, file_offsets[index], numPixelsPerByte, bit_file_data[index]))
		{
			return -5; // Invalid data
		}
	}

	return 0;
}

int UltimaVResource::ParseBitFile(std::vector<U5ImageData> &bit_file_data, std::vector<unsigned char>& data)
{
	if (data.size() < 2)
	{
		return -1;
	}
	size_t curPos = 0;
	uint32_t numImages = ReadInt16(data.begin(), curPos);
	if (numImages == 0 || numImages > 32)
	{
		return -2;
	}
	bit_file_data.resize(numImages);
	std::vector<size_t> file_offsets;
	if (0 != ReadOffsets(data, 2, numImages, file_offsets, curPos))
	{
		return -3;
	}
	for (uint32_t index = 0; index < numImages; index++)
	{
		if (file_offsets[index] == 0)
		{
			return -4; // Only dungeons have missing offsets
		}
		if (0 != ReadImage(data, file_offsets[index], 8, bit_file_data[index]))
		{
			return -5; // Invalid data
		}
	}

	return 0;
}

int UltimaVResource::ParseCharacterFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int width, int height)
{
	const int NUM_CHARACTERS = 128;

	bit_file_data.resize(NUM_CHARACTERS);

	size_t curOffset = 0;
	for (int index = 0; index < NUM_CHARACTERS; index++)
	{
		bit_file_data[index].width = width;
		bit_file_data[index].height = height;
		bit_file_data[index].mode = 8;
		bit_file_data[index].pixel_data.resize(static_cast<size_t>(width * height));
		size_t curPos = 0;

		for (size_t byteIndex = 0; byteIndex < (static_cast<size_t>(width * height)) / 8; byteIndex++)
		{
			if (curOffset >= data.size())
			{
				return -1;
			}
			unsigned char curByte = data[curOffset];

			for (int bitIndex = 0; bitIndex < 8; bitIndex++)
			{
				unsigned char curColor = (static_cast<int>(curByte) >> ((7 - bitIndex)) & 0x1);
				bit_file_data[index].pixel_data[curPos] = curColor;
				curPos++;
			}

			curOffset++;
		}
	}
	return 0;
}

int UltimaVResource::LoadBuffer(const std::string strFile, std::vector<unsigned char>& buffer)
{
	std::filesystem::path file_path = m_options->m_game_directory / strFile;
	if (!std::filesystem::exists(file_path))
	{
		return -1;
	}
	std::uintmax_t file_size = std::filesystem::file_size(file_path);
	buffer.resize(file_size);
	std::ifstream file(file_path, std::ios::binary);
	file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
	file.close();

	return 0;
}

int UltimaVResource::LoadPathFile()
{
	int ret = LoadBuffer("BRITISH.PTH", m_PathFileData);

	return ret;
}

int UltimaVResource::LoadBitFiles()
{
	const std::vector<std::string> file_names = { "BRITISH.BIT", "TITLE.BIT", "WD.BIT" };
	m_BitFileData.resize(file_names.size());
	
	int array_pos = 0;

	for (std::string strFile : file_names)
	{
		std::vector<unsigned char> buffer;
		int ret = LoadBuffer(strFile, buffer);

		if (0 != ret)
		{
			return ret;
		}

		if (strFile == file_names[2])
		{
			auto& curVec = m_BitFileData[static_cast<std::uint_fast8_t>(BitFileNames::WD)];
			if (0 != ParseBitFile(curVec, buffer))
			{
				return -1;
			}
		}
		else
		{
			auto& curVec = m_BitFileData[array_pos];
			std::vector<unsigned char> buffer_lzw;
			if (!LzwDecompressor::Extract(buffer, buffer_lzw))
			{
				return -2;
			}
			if (0 != ParseBitFile(curVec, buffer_lzw))
			{
				return -1;
			}
		}
		array_pos++;
	}

	return 0;
}

int UltimaVResource::LoadProportionalFont()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("PROPORT.PCS", buffer);

	if (0 != ret)
	{
		return ret;
	}

	std::vector<unsigned char> buffer_lzw;
	if (!LzwDecompressor::Extract(buffer, buffer_lzw))
	{
		return -2;
	}

	size_t curPos = 0;
	std::vector<uint32_t> offsets;

	uint32_t num_letters = ReadInt16(buffer_lzw.begin(), curPos);
	m_ProportionalFontData.resize(num_letters);
	offsets.resize(num_letters);
	
	for (uint32_t index = 0; index < num_letters; index++)
	{
		if (buffer_lzw.size() <= curPos + 2)
		{
			return -3;
		}
		offsets[index] = ReadInt16(buffer_lzw.begin(), curPos);
	}

	size_t curOffset = 0;

	for (uint32_t index = 0; index < num_letters; index++)
	{
		m_ProportionalFontData[index].height = 8;
		m_ProportionalFontData[index].width = 8;
		m_ProportionalFontData[index].mode = 8;
		m_ProportionalFontData[index].pixel_data.resize(static_cast<size_t>(64));

		curOffset = offsets[index];

		// width + height + data
		if (curOffset + 12 > buffer_lzw.size())
		{
			return -1;
		}

		m_ProportionalFontData[index].real_width = ReadInt16(buffer_lzw.begin(), curOffset);
		m_ProportionalFontData[index].real_height = ReadInt16(buffer_lzw.begin(), curOffset);

		curPos = 0;
		for (int yIndex = 0; yIndex < 8; yIndex++)
		{
			unsigned char curByte = buffer_lzw[curOffset];
			for (int bitIndex = 0; bitIndex < 8; bitIndex++)
			{
				unsigned char curColor = (static_cast<int>(curByte) >> ((7 - bitIndex)) & 0x1);
				m_ProportionalFontData[index].pixel_data[curPos] = curColor;
				curPos++;
			}
			curOffset++;
		}
	}

	return 0;
}

int UltimaVResource::LoadCharacterSets()
{
	const int NUM_CHARACTERS = 128;
	const std::vector<std::string> file_names = { "IBM", "RUNES" };
	const std::vector<std::string> file_extensions = { ".CH", ".HCS" };
	m_CharacterSetsData.resize(file_names.size());
	const int WIDTH[2] = { 8, 16 };
	const int HEIGHT[2] = { 8, 12 };
	for (size_t index = 0; index < file_names.size(); index++)
	{
		m_CharacterSetsData[index].resize(file_extensions.size());
		for (size_t ext_index = 0; ext_index < file_names.size(); ext_index++)
		{
			std::vector<unsigned char> buffer;
			int ret = LoadBuffer(file_names[index] + file_extensions[ext_index], buffer);

			if (0 != ret)
			{
				return ret;
			}

			m_CharacterSetsData[index][ext_index].resize(NUM_CHARACTERS);

			if (0 != ParseCharacterFile(m_CharacterSetsData[index][ext_index], buffer, WIDTH[ext_index], HEIGHT[ext_index]))
			{
				return -2;
			}
		}
	}
	return 0;
}

int UltimaVResource::LoadMaskedImages()
{
	std::string strExt(".16");
	int numPixelPerByte = 2;
	if (m_render_mode == RenderMode::CGA)
	{
		strExt = std::string(".4");
		numPixelPerByte = 4;
	}
	const std::vector<std::string> file_names = { "ITEMS", "MON0", "MON1", "MON2", "MON3", "MON4", "MON5", "MON6", "MON7" };

	m_ImageMaskFileData.resize(file_names.size());

	for (size_t index = 0; index < file_names.size(); index++)
	{
		std::vector<unsigned char> buffer;
		int ret = LoadBuffer(file_names[index] + strExt, buffer);

		if (0 != ret)
		{
			return ret;
		}

		std::vector<unsigned char> buffer_lzw;
		if (!LzwDecompressor::Extract(buffer, buffer_lzw))
		{
			return -2;
		}
		auto& curVec = m_ImageMaskFileData[index];

		if (0 != ParseMaskFile(curVec, buffer_lzw, numPixelPerByte))
		{
			return -1;
		}
	}
	return 0;
}

int UltimaVResource::LoadDungeonImages()
{
	std::string strExt(".16");
	int numPixelPerByte = 2;
	if (m_render_mode == RenderMode::CGA)
	{
		strExt = std::string(".4");
		numPixelPerByte = 4;
	}
	const std::vector<std::string> file_names = { "DNG1", "DNG2", "DNG3" };

	m_ImageDungeonFileData.resize(file_names.size());

	for (size_t index = 0; index < file_names.size(); index++)
	{
		std::vector<unsigned char> buffer;
		int ret = LoadBuffer(file_names[index] + strExt, buffer);

		if (0 != ret)
		{
			return ret;
		}

		std::vector<unsigned char> buffer_lzw;
		if (!LzwDecompressor::Extract(buffer, buffer_lzw))
		{
			return -2;
		}
		auto& curVec = m_ImageDungeonFileData[index];

		if (0 != Parse16File(curVec, buffer_lzw, numPixelPerByte))
		{
			return -1;
		}
	}

	return 0;
}

int UltimaVResource::Load16Images()
{
	std::string strExt(".16");
	int numPixelPerByte = 2;
	if (m_render_mode == RenderMode::CGA)
	{
		strExt = std::string(".4");
		numPixelPerByte = 4;
	}

	const std::vector<std::string> file_names = { "CREATE", "END1", "END2", "ENDSC", "STARTSC",
			"STORY1", "STORY2", "STORY3", "STORY4", "STORY5", "STORY6", "TEXT", "ULTIMA" };

	m_Image16FileData.resize(file_names.size());

	for (size_t index = 0; index < file_names.size(); index++)
	{
		std::vector<unsigned char> buffer;
		int ret = LoadBuffer(file_names[index] + strExt, buffer);

		if (0 != ret)
		{
			return ret;
		}

		std::vector<unsigned char> buffer_lzw;
		if (!LzwDecompressor::Extract(buffer, buffer_lzw))
		{
			return -2;
		}
		auto& curVec = m_Image16FileData[index];

		if (0 != Parse16File(curVec, buffer_lzw, numPixelPerByte))
		{
			return -1;
		}
	}

	return 0;
}

void UltimaVResource::LoadStoryText(const std::vector<unsigned char>& buffer, size_t pos, std::vector<unsigned char>& text)
{
	for (size_t curPos = pos; curPos < buffer.size(); curPos++)
	{
		if (buffer[curPos] != 0)
		{
			text.push_back(buffer[curPos]);
		}
		else
		{
			return;
		}
	}
}

int UltimaVResource::LoadEnding(std::vector<unsigned char>& data_buffer)
{
	const size_t NUM_STORIES = 6;
	const size_t X_LEFT_PARAGRAPH_OFFSET = 0x3db6;
	const size_t X_RIGHT_PARAGRAPH_OFFSET = 0x3dc2;
	const size_t STORY_OFFSET = 0x3dda;
	const size_t Y_PARAGRAPH_1_EXTENT = 0x3de6;
	const size_t Y_PARAGRAPH_2_EXTENT = 0x3dec;
	const size_t X_FIRST_LINE_POS = 0x3df2;
	const size_t Y_START_POS = 0x3df8;
	const size_t STORY_IMAGE_INDEX = 0x3dfe;
	const size_t STORY_NUMBER = 0x3e04;
	const size_t STORY_PIC_X = 0x3e0a;
	const size_t STORY_PIC_Y = 0x3e10;
	const size_t STORY_ACTION = 0x3e16;

	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("END.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	for (size_t index = 0; index < NUM_STORIES; index++)
	{
		size_t temppos = STORY_OFFSET + index * 2;
		m_data.ending_text[index].text_offset = ReadInt16(data_buffer.begin(), temppos);
		if (m_data.ending_text[index].text_offset >= buffer.size())
		{
			return -2;
		}

		LoadStoryText(buffer, m_data.ending_text[index].text_offset, m_data.ending_text[index].text);
		m_data.ending_text[index].image_index = data_buffer[STORY_IMAGE_INDEX + index];
		m_data.ending_text[index].story_number = data_buffer[STORY_NUMBER + index];
		m_data.ending_text[index].picture_x = data_buffer[STORY_PIC_X + index];
		m_data.ending_text[index].picture_y = data_buffer[STORY_PIC_Y + index];
		m_data.ending_text[index].action = data_buffer[STORY_ACTION + index];
		m_data.ending_text[index].text_y_pos = data_buffer[Y_START_POS + index];
		m_data.ending_text[index].first_line_offset = data_buffer[X_FIRST_LINE_POS + index];
		m_data.ending_text[index].paragraph[0].y_extent = data_buffer[Y_PARAGRAPH_1_EXTENT + index];
		m_data.ending_text[index].paragraph[1].y_extent = data_buffer[Y_PARAGRAPH_2_EXTENT + index];
		m_data.ending_text[index].paragraph[0].text_left_pos = data_buffer[X_LEFT_PARAGRAPH_OFFSET + (index * 2)];
		m_data.ending_text[index].paragraph[1].text_left_pos = data_buffer[X_LEFT_PARAGRAPH_OFFSET + (index * 2) + 1];
		size_t curPos = X_RIGHT_PARAGRAPH_OFFSET + (index * 4);
		auto iter = data_buffer.begin() + curPos;
		uint32_t val = ReadInt16(data_buffer.begin(), curPos);
		m_data.ending_text[index].paragraph[0].text_right_pos = val;
		curPos = X_RIGHT_PARAGRAPH_OFFSET + (index * 4) + 2;
		iter = data_buffer.begin() + curPos;
		val = ReadInt16(data_buffer.begin(), curPos);
		m_data.ending_text[index].paragraph[1].text_right_pos = val;
	}

	return 0;
}

int UltimaVResource::LoadStory(std::vector<unsigned char> &data_buffer)
{
	const size_t INTRO_OFFSET_IN_DATA_1 = 0x2f41;
	const size_t INTRO_OFFSET_IN_DATA_2 = 0x2f6f;

	const size_t NUM_STORIES = 21;
	const size_t X_LEFT_PARAGRAPH_OFFSET = 0x2fa8;
	const size_t X_RIGHT_PARAGRAPH_OFFSET = 0x2fd2;
	const size_t STORY_OFFSET = 0x3026;
	const size_t Y_PARAGRAPH_1_EXTENT = 0x3050;
	const size_t Y_PARAGRAPH_2_EXTENT = 0x3066;
	const size_t X_FIRST_LINE_POS = 0x307c;
	const size_t Y_START_POS = 0x3092;
	const size_t STORY_IMAGE_INDEX = 0x30a8;
	const size_t STORY_NUMBER = 0x30be;
	const size_t STORY_PIC_X = 0x30d4;
	const size_t STORY_PIC_Y = 0x30ea;
	const size_t STORY_ACTION = 0x3100;

	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("STORY.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	LoadStoryText(data_buffer, INTRO_OFFSET_IN_DATA_1, m_data.intro_string_1);
	LoadStoryText(data_buffer, INTRO_OFFSET_IN_DATA_2, m_data.intro_string_2);

	for (size_t index = 0; index < NUM_STORIES; index++)
	{
		size_t temppos = STORY_OFFSET + index * 2;
		m_data.story_text[index].text_offset = ReadInt16(data_buffer.begin(), temppos);
		if (m_data.story_text[index].text_offset >= buffer.size())
		{
			return -2;
		}

		LoadStoryText(buffer, m_data.story_text[index].text_offset, m_data.story_text[index].text);
		m_data.story_text[index].image_index = data_buffer[STORY_IMAGE_INDEX + index];
		m_data.story_text[index].story_number = data_buffer[STORY_NUMBER + index];
		m_data.story_text[index].picture_x = data_buffer[STORY_PIC_X + index];
		m_data.story_text[index].picture_y = data_buffer[STORY_PIC_Y + index];
		m_data.story_text[index].action = data_buffer[STORY_ACTION + index];
		m_data.story_text[index].text_y_pos = data_buffer[Y_START_POS + index];
		m_data.story_text[index].first_line_offset = data_buffer[X_FIRST_LINE_POS + index];
		m_data.story_text[index].paragraph[0].y_extent = data_buffer[Y_PARAGRAPH_1_EXTENT + index];
		m_data.story_text[index].paragraph[1].y_extent = data_buffer[Y_PARAGRAPH_2_EXTENT + index];
		m_data.story_text[index].paragraph[0].text_left_pos = data_buffer[X_LEFT_PARAGRAPH_OFFSET + (index * 2)];
		m_data.story_text[index].paragraph[1].text_left_pos = data_buffer[X_LEFT_PARAGRAPH_OFFSET + (index * 2) + 1];
		size_t curPos = X_RIGHT_PARAGRAPH_OFFSET + (index * 4);
		auto iter = data_buffer.begin() + curPos;
		uint32_t val = ReadInt16(data_buffer.begin(), curPos);
		m_data.story_text[index].paragraph[0].text_right_pos = val;
		curPos = X_RIGHT_PARAGRAPH_OFFSET + (index * 4) + 2;
		iter = data_buffer.begin() + curPos;
		val = ReadInt16(data_buffer.begin(), curPos);
		m_data.story_text[index].paragraph[1].text_right_pos = val;
	}

	return 0;
}

int UltimaVResource::LoadDungeonSigns(const std::vector<unsigned char>& buffer)
{
	const size_t NUM_DUNGEONS = 8;
	const size_t NUM_SIGNS = 11;
	m_DungeonSignData.resize(NUM_DUNGEONS);

	std::vector<int> sign_x;
	std::vector<int> sign_y;
	std::vector<std::string> sign_text;

	const size_t X_LOC = 0x2e08;
	const size_t Y_LOC = 0x2e14;
	const size_t TEXT_OFFSET_LOC = 0x2e20;
	const size_t START_INDEX = 0x2df8;
	const size_t NUM_SIGNS_INDEX = 0x2e00;

	for (size_t index = 0; index < NUM_SIGNS; index++)
	{
		sign_x.emplace_back(buffer[X_LOC + index]);
		sign_y.emplace_back(buffer[Y_LOC + index]);
		size_t curPos = 0;
		uint32_t tempoffset = ReadInt16(buffer.begin() + TEXT_OFFSET_LOC + (static_cast<size_t>(index * 2)), curPos);
		tempoffset += 0x10;
		std::string tempstr = ReadNextString(buffer.begin() + tempoffset, buffer.end());
		sign_text.push_back(tempstr);
	}

	for (size_t index = 0; index < NUM_DUNGEONS; index++)
	{
		int start = buffer[START_INDEX + index];
		int num = buffer[NUM_SIGNS_INDEX + index];
		for (int sign_index = start; sign_index < start + num; sign_index++)
		{
			if (sign_index < sign_x.size())
			{
				m_DungeonSignData[index].emplace_back(sign_x[sign_index], sign_y[sign_index], 0, sign_text[sign_index]);
			}
		}
	}

	return 0;
}

std::string UltimaVResource::ReadNextString(std::vector<unsigned char>::const_iterator data, std::vector<unsigned char>::const_iterator end)
{
	// Just a safety check to avoid trying to overflow a buffer
	const int MAX_NAME_SIZE = 256;
	std::string retval;
	// Limit 
	for (int index = 0; index < MAX_NAME_SIZE; index++)
	{
		if (data + index == end)
		{
			break;
		}
		if (*(data + index) == 0)
		{
			return retval;
		}
		retval += *(data + index);
	}

	return retval;
}

bool UltimaVResource::ReadStrings(const std::vector<unsigned char>& buffer, std::vector<std::string>& str_vec, size_t start_pos, size_t end_pos)
{
	if (buffer.size() <= end_pos)
	{
		return false;
	}

	std::string strValue;
	for (size_t index = start_pos; index <= end_pos; index++)
	{
		if (buffer[index] == 0)
		{
			if (strValue.size() > 0)
			{
				str_vec.push_back(strValue);
				strValue.clear();
			}
		}
		else
		{
			strValue += buffer[index];
		}
	}
	return true;
}

int UltimaVResource::LoadDataOvl()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("DATA.OVL", buffer);

	if (0 != ret)
	{
		return ret;
	}

	// The file should be 48464 (0xbd50) bytes long, but who knows if someone will ever hack the
	// file, so just have a basic sanity check
	if (buffer.size() < 0xA000)
	{
		return -2;
	}
	if (0 != LoadStory(buffer))
	{
		return -3;
	}
	if (0 != LoadEnding(buffer))
	{
		return -4;
	}
	if (!ReadStrings(buffer, m_data.intro_strings, 0x311c, 0x3664))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.intro_demo_string, 0xa020, 0xa053))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_0, 0x52, 0x129a))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_1, 0x266a, 0x28d5))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_2, 0x2956, 0x2bd0))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_3, 0x2c8c, 0x2df8))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_4, 0x6aea, 0x6d56))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_5, 0x6d84, 0x7010))
	{
		return -5;
	}
	if (!ReadStrings(buffer, m_data.game_strings_6, 0x702a, 0x70e8))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_7, 0x70f2, 0x71d1))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_8, 0x71dc, 0x7208))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_9, 0x721c, 0x7384))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_10, 0x7398, 0x74f5))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_11, 0x750a, 0x77f3))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_12, 0x7808, 0x7850))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_13, 0x785c, 0x7a19))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_14, 0x7a26, 0x7f09))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_15, 0x7f26, 0x820e))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_16, 0x8216, 0x848f))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_17, 0x84aa, 0x9215))
	{
		return -5;
	}

	if (!ReadStrings(buffer, m_data.game_strings_18, 0x9338, 0xa45a))
	{
		return -5;
	}

	/*if (!ReadStrings(buffer, m_data.intro_strings, 0x750a, 0xa459))
	{
		return -5;
	}*/
	const size_t MAP_CHUNK_LOC = 0x3886;
	const size_t MAP_NUM_CHUNKS = 0x100;

	std::copy_n(buffer.begin() + MAP_CHUNK_LOC, MAP_NUM_CHUNKS, m_data.map_chunks.begin());

	const size_t LOCATION_X = 0x1e9a;
	const size_t LOCATION_Y = 0x1ec2;
	const size_t LOCATION_Z = 0x1e2a;
	const size_t LOCATION_NAME_OFFSET = 0x1e4a;

	for (size_t index = 0; index < m_data.location_info.size(); index++)
	{
		m_data.location_info[index].first = buffer[LOCATION_X + index];
		m_data.location_info[index].second = buffer[LOCATION_Y + index];

		if (index < 32)
		{
			m_data.location_z_index[index] = buffer[LOCATION_Z + index];
			if (m_data.location_z_index[index] >= 16)
			{
				return -6;
			}
		}

		uint32_t name_offset;
		size_t curPos = LOCATION_NAME_OFFSET + (2 * index);
		name_offset = ReadInt16(buffer.begin(), curPos);
		name_offset += 0x10;
		m_data.location_names[index] = ReadNextString(buffer.begin() + name_offset, buffer.end());
	}

	if (0 != LoadDungeonSigns(buffer))
	{
		return -7;
	}

	return 0;
}

int UltimaVResource::LoadTiles()
{
	int bitInc;
	std::string strExt;
	int numPixelsPerByte;
	int modnum;
	if (m_render_mode == RenderMode::CGA)
	{
		strExt = std::string(".4");
		numPixelsPerByte = 4;
		bitInc = 2;
		modnum = 0b11;
	}
	else
	{
		strExt = std::string(".16");
		numPixelsPerByte = 2;
		bitInc = 4;
		modnum = 0b1111;
	}

	std::vector<unsigned char> buffer;
	int ret = LoadBuffer(std::string("TILES") + strExt, buffer);

	if (0 != ret)
	{
		return ret;
	}

	std::vector<unsigned char> buffer_lzw;
	if (!LzwDecompressor::Extract(buffer, buffer_lzw))
	{
		return -2;
	}
	// Verify file size.  This is the path for base textures, not custom ones
	if (m_render_mode == RenderMode::CGA)
	{
		if (buffer_lzw.size() != 0x8000)
		{
			return -3;
		}
	}
	else
	{
		if (buffer_lzw.size() != 0x10000)
		{
			return -3;
		}
	}
	const size_t NUM_TILES = 512;
	const size_t TILE_WIDTH = 16;
	const size_t TILE_HEIGHT = 16;
	for (size_t index = 0; index < NUM_TILES; index++)
	{
		m_Tiles.emplace_back();
		m_Tiles.back().pixel_data.resize(TILE_WIDTH * TILE_HEIGHT);
		m_Tiles.back().mode = numPixelsPerByte;
		m_Tiles.back().width = TILE_WIDTH;
		m_Tiles.back().height = TILE_HEIGHT;
		size_t curPos = index * (TILE_WIDTH / numPixelsPerByte) * TILE_HEIGHT;

		for (size_t indexY = 0; indexY < TILE_HEIGHT; indexY++)
		{
			for (size_t indexX = 0; indexX < TILE_WIDTH / numPixelsPerByte; indexX++)
			{
				unsigned char curByte = buffer_lzw[curPos];
				for (int bitIndex = 0; bitIndex < 8; bitIndex += bitInc)
				{
					int curColor = (static_cast<int>(curByte) >> ((8 - bitInc) - bitIndex)) & modnum;
					if (indexX * numPixelsPerByte + (bitIndex / bitInc) < m_Tiles.back().width)
					{
						m_Tiles.back().pixel_data[static_cast<size_t>(indexY * m_Tiles.back().width + indexX * numPixelsPerByte + (bitIndex / bitInc))] = static_cast<unsigned char>(curColor);
					}
				}
				curPos++;
			}
		}
	}
	
	return 0;
}

int UltimaVResource::LoadMiscMaps()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("MISCMAPS.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	// Invalid file
	if(buffer.size() < 0x4C0)
	{
		return -2;
	}

	const size_t CUTSCENE_PITCH = 16;
	const size_t CUTSCENE_SIZE = CUTSCENE_HEIGHT * CUTSCENE_PITCH;

	for (size_t index = 0; index < NUM_CUTSCENES; index++)
	{
		for (size_t indexRow = 0; indexRow < CUTSCENE_HEIGHT; indexRow++)
		{
			for (size_t indexCol = 0; indexCol < CUTSCENE_WIDTH; indexCol++)
			{
				m_CutsceneMap[index][indexCol][indexRow] = buffer[(CUTSCENE_SIZE * index) + (indexRow * CUTSCENE_PITCH) + indexCol];
			}
		}
	}

	const size_t DEMO_PITCH = 32;
	const size_t DEMO_SIZE = DEMO_HEIGHT * DEMO_PITCH;
	const size_t DEMO_START = 0x2C0;

	for (size_t index = 0; index < NUM_DEMOS; index++)
	{
		for (size_t indexRow = 0; indexRow < DEMO_HEIGHT; indexRow++)
		{
			for (size_t indexCol = 0; indexCol < DEMO_WIDTH; indexCol++)
			{
				m_DemoMap[index][indexCol][indexRow] = buffer[DEMO_START + (DEMO_SIZE * index) + (indexRow * DEMO_PITCH) + indexCol];
			}
		}
	}

	const size_t DEMO_INSTRUCTION_START = 0x4C0;
	for (size_t index = DEMO_INSTRUCTION_START; index < buffer.size(); index++)
	{
		IntroScriptInstruction curInstruction;
		if (buffer[index] > 0xF)
		{
			return -1;
		}
		curInstruction.instruction = buffer[index];
		size_t variable_count = 0;
		switch (curInstruction.instruction)
		{
		case 0: //Set character
			variable_count = 4;
			break;
		case 2: // Move character with next instruction
		case 4: // Open moongate
		case 0xB: // Shoot
		case 0xD: // Move character
			variable_count = 2;
			break;
		case 1:
		case 3: // Set delay
		case 6: // Set map
		case 7: // Fade In
		case 8: // Fade Out
		case 0xE: // Loop
			variable_count = 1;
			break;
		case 0xA: // Load Tile
			variable_count = 3;
			break;
		default:
			variable_count = 0;
			break;
		}
		// Exceeded file size
		if (index + variable_count >= buffer.size())
		{
			return -2;
		}
		for (size_t tempIndex = 1; tempIndex <= variable_count; tempIndex++)
		{
			curInstruction.data[tempIndex - 1] = buffer[index + tempIndex];
		}
		index += variable_count;
		m_IntroInstructions.push_back(curInstruction);
	}

	return 0;
}

int UltimaVResource::LoadMapChunk(unsigned char cur_chunk_val, size_t curChunkX, size_t curChunkY, std::vector<std::vector<unsigned char>> &map, const std::vector<unsigned char>& buffer)
{
	const size_t CHUNK_WIDTH = 16;
	const size_t CHUNK_HEIGHT = 16;
	const size_t CHUNK_SIZE = CHUNK_WIDTH * CHUNK_HEIGHT;

	map.resize(256);
	for (auto& curRow : map)
	{
		curRow.resize(256);
	}

	if (cur_chunk_val == 0xFF) // All water
	{
		for (size_t indexY = 0; indexY < CHUNK_HEIGHT; indexY++)
		{
			for (size_t indexX = 0; indexX < CHUNK_WIDTH; indexX++)
			{
				map[(curChunkX * CHUNK_WIDTH) + indexX][(curChunkY * CHUNK_HEIGHT) + indexY] = 0x01;
			}
		}
	}
	else
	{
		if (buffer.size() < (static_cast<size_t>(cur_chunk_val + 1)) * CHUNK_SIZE)
		{
			return -1;
		}
		for (size_t indexY = 0; indexY < CHUNK_HEIGHT; indexY++)
		{
			for (size_t indexX = 0; indexX < CHUNK_WIDTH; indexX++)
			{
				map[(curChunkX * CHUNK_WIDTH) + indexX][(curChunkY * CHUNK_HEIGHT) + indexY] = buffer[(cur_chunk_val * CHUNK_SIZE) + (indexY * CHUNK_WIDTH) + indexX];
			}
		}
	}
	return 0;
}

int UltimaVResource::LoadNPCs(MapTypes map_type)
{
	std::string strStoryFile;
	
	switch (map_type)
	{
	case MapTypes::Castle:
		strStoryFile = std::string("CASTLE.NPC");
		break;
	case MapTypes::Dwelling:
		strStoryFile = std::string("DWELLING.NPC");
		break;
	case MapTypes::Keep:
		strStoryFile = std::string("KEEP.NPC");
		break;
	case MapTypes::Town:
		strStoryFile = std::string("TOWNE.NPC");
		break;
	default:
		return -1;
	}

	std::vector<unsigned char> buffer;
	int ret = LoadBuffer(strStoryFile, buffer);

	if (0 != ret)
	{
		return ret;
	}

	if (buffer.size() != 4608)
	{
		return -2;
	}
	int map_num = static_cast<int>(map_type);
	auto& curMap = m_data.npc_info[map_num];
	size_t curPos = 0;

	for (int index = 0; index < 8; index++)
	{
		// Load the schedule
		for (int schedule_index = 0; schedule_index < 32; schedule_index++)
		{
			for (int AI_index = 0; AI_index < 3; AI_index++)
			{
				curMap.info[index].data[schedule_index].schedule.AI_types[AI_index] = buffer[curPos];
				curPos++;
			}
			for (int x_index = 0; x_index < 3; x_index++)
			{
				curMap.info[index].data[schedule_index].schedule.x_coordinates[x_index] = buffer[curPos];
				curPos++;
			}
			for (int y_index = 0; y_index < 3; y_index++)
			{
				curMap.info[index].data[schedule_index].schedule.y_coordinates[y_index] = buffer[curPos];
				curPos++;
			}
			for (int z_index = 0; z_index < 3; z_index++)
			{
				curMap.info[index].data[schedule_index].schedule.z_coordinates[z_index] = static_cast<int8_t>(buffer[curPos]);
				curPos++;
				//std::cout << std::to_string(curMap.info[index].schedule[schedule_index].z_coordinates[z_index]) << std::endl;
			}
			for (int times_index = 0; times_index < 4; times_index++)
			{
				curMap.info[index].data[schedule_index].schedule.times[times_index] = buffer[curPos];
				curPos++;
			}
		}
		// Load the NPC type
		for (int schedule_index = 0; schedule_index < 32; schedule_index++)
		{
			curMap.info[index].data[schedule_index].type = buffer[curPos];
			curPos++;
		}
		// Load the dialog number
		for (int schedule_index = 0; schedule_index < 32; schedule_index++)
		{
			curMap.info[index].data[schedule_index].dialog_number = buffer[curPos];
			curPos++;
		}
	}

	return 0;
}

int UltimaVResource::LoadMap(MapTypes map_type)
{
	std::string strStoryFile;
	switch (map_type)
	{
	case MapTypes::Castle:
		strStoryFile = std::string("CASTLE.DAT");
		break;
	case MapTypes::Dwelling:
		strStoryFile = std::string("DWELLING.DAT");
		break;
	case MapTypes::Keep:
		strStoryFile = std::string("KEEP.DAT");
		break;
	case MapTypes::Town:
		strStoryFile = std::string("TOWNE.DAT");
		break;
	default:
		return -1;
	}

	std::vector<unsigned char> buffer;
	int ret = LoadBuffer(strStoryFile, buffer);

	if (0 != ret)
	{
		return ret;
	}

	if (buffer.size() != 16384)
	{
		return -2;
	}

	std::vector<std::vector<std::vector<std::vector<unsigned char>>>*> map_list = { &m_data.town_maps, &m_data.dwelling_maps, &m_data.castle_maps,  &m_data.keep_maps };

	const int NUM_LEVELS = 16;
	const int NUM_X = 32;
	const int NUM_Y = 32;
	auto& cur_map = map_list[static_cast<int>(map_type)];
	cur_map->resize(NUM_LEVELS);
	for (size_t curLevel = 0; curLevel < NUM_LEVELS; curLevel++)
	{
		size_t data_offset = curLevel * NUM_X * NUM_Y;
		(*cur_map)[curLevel].resize(NUM_X);
		for (size_t curX = 0; curX < NUM_X; curX++)
		{
			(*cur_map)[curLevel][curX].resize(NUM_Y);
			for (size_t curY = 0; curY < NUM_Y; curY++)
			{
				size_t curPos = data_offset + (curY * NUM_X) + curX;
				(*cur_map)[curLevel][curX][curY] = buffer[curPos];
			}
		}
	}

	return 0;
}

int UltimaVResource::LoadWorldMap()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("BRIT.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	const size_t CHUNK_WIDTH = 16;
	const size_t CHUNK_HEIGHT = 16;

	for (size_t curChunkY = 0; curChunkY < CHUNK_HEIGHT; curChunkY++)
	{
		for (size_t curChunkX = 0; curChunkX < CHUNK_WIDTH; curChunkX++)
		{
			size_t cur_chunk = curChunkY * CHUNK_WIDTH + curChunkX;
			unsigned char cur_chunk_val = m_data.map_chunks[cur_chunk];
			if (0 != LoadMapChunk(cur_chunk_val, curChunkX, curChunkY, m_data.world_map, buffer))
			{
				return -1;
			}
		}
	}

	return 0;
}

int UltimaVResource::LoadUnderworldMap()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("UNDER.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	const size_t CHUNK_WIDTH = 16;
	const size_t CHUNK_HEIGHT = 16;

	size_t cur_chunk_val = 0;

	for (size_t curChunkY = 0; curChunkY < CHUNK_HEIGHT; curChunkY++)
	{
		for (size_t curChunkX = 0; curChunkX < CHUNK_WIDTH; curChunkX++)
		{
			if (0 != LoadMapChunk(static_cast<unsigned char>(cur_chunk_val), curChunkX, curChunkY, m_data.underworld_map, buffer))
			{
				return -1;
			}
			cur_chunk_val++;
		}
	}

	return 0;
}

int UltimaVResource::LoadDungeons()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("DUNGEON.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	const int NUM_LEVELS = 64;
	const int NUM_X = 8;
	const int NUM_Y = 8;
	m_data.dungeon_maps.resize(NUM_LEVELS);
	for (size_t curLevel = 0; curLevel < NUM_LEVELS; curLevel++)
	{
		size_t data_offset = curLevel * NUM_X * NUM_Y;
		m_data.dungeon_maps[curLevel].resize(NUM_X);
		for (size_t curX = 0; curX < NUM_X; curX++)
		{
			m_data.dungeon_maps[curLevel][curX].resize(NUM_Y);
			for (size_t curY = 0; curY < NUM_Y; curY++)
			{
				size_t curPos = data_offset + (curY * NUM_X) + curX;
				//m_data.dungeon_maps[curLevel][curX][curY] = buffer[curPos];
				uint8_t upper_nibble = (buffer[curPos] >> 4) & 0xF;
				uint8_t lower_nibble = buffer[curPos] & 0xF;
				m_data.dungeon_maps[curLevel][curX][curY] = { upper_nibble, lower_nibble };
			}
		}
	}

	return 0;
}

int UltimaVResource::LoadSigns()
{
	std::vector<unsigned char> buffer;
	int ret = LoadBuffer("SIGNS.DAT", buffer);

	if (0 != ret)
	{
		return ret;
	}

	const int NUM_SIGNS = 0x21;

	std::vector<size_t> file_offsets;
	size_t curPos = 0;
	if (0 != ReadOffsets(buffer, 2, NUM_SIGNS, file_offsets, curPos))
	{
		return -3;
	}

	m_SignData.resize(NUM_SIGNS);

	for (size_t index = 0; index < NUM_SIGNS; index++)
	{
		if (file_offsets[index] == 0)
		{
			continue;
		}
		curPos = file_offsets[index];
		while (curPos < buffer.size())
		{
			uint8_t loc = buffer[curPos];
			if (loc != index)
			{
				break;
			}
			curPos++;
			if (curPos >= buffer.size())
			{
				return -1;
			}
			uint8_t z = buffer[curPos];
			curPos++;
			if (curPos >= buffer.size())
			{
				return -1;
			}
			uint8_t x = buffer[curPos];
			curPos++;
			if (curPos >= buffer.size())
			{
				return -1;
			}
			uint8_t y = buffer[curPos];
			curPos++;
			if (curPos >= buffer.size())
			{
				return -1;
			}
			std::vector<unsigned char>::iterator iter = buffer.begin() + curPos;
			std::string strOut = ReadNextString(iter, buffer.end());
			SwapCharset(strOut);
			curPos += strOut.size();
			if (curPos >= buffer.size())
			{
				return -1;
			}
			m_SignData[index].emplace_back(x, y, z, strOut);
			curPos++;
		}
	}

	return 0;
}

void UltimaVResource::SwapCharset(std::string& curString)
{
	for (size_t index = 0; index < curString.size(); index++)
	{
		unsigned char c = static_cast<unsigned char>(curString[index]);
		if (c >= 128)
		{
			c -= 128;
		}
		else
		{
			if (c != 0xa)
			{
				c += 128;
			}
		}
		curString[index] = static_cast<char>(c);

		if (index > 0)
		{
			if (c == 0xa7)
			{
				unsigned char d = static_cast<unsigned char>(curString[index - 1]);
				if (d == 0xa6)
				{
					char cline = static_cast<char>(0xec);
					curString[index - 1] = cline;
					curString[index] = cline;
				}
			}
		}
	}
}
