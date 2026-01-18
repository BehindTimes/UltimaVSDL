#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>

#include "UltimaVResource.h"
#include "LzwDecompressor.h"
#include <cstdint>
#include <string>
#include <vector>

static const std::filesystem::path GAME_DIRECTORY("G:/source/UltimaVSDL/u5data");

UltimaVResource::UltimaVResource()
{
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
	if (0 != LoadCharacterSets())
	{
		return -1;
	}

	return 0;
}

uint32_t UltimaVResource::ReadInt16(std::vector<unsigned char>::iterator data, size_t& curPos)
{
	uint32_t retVal = (static_cast<uint32_t>(data[curPos + 1]) << 8) + static_cast<uint32_t>(data[curPos]);
	curPos += 2;
	return retVal;
}

uint32_t UltimaVResource::ReadInt32(std::vector<unsigned char>::iterator data, size_t& curPos)
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
	int byteInc = 0;
	if (numPixelsPerByte == 8)
	{
		modnum = 0b1;
		byteInc = 1;
	}
	else if (numPixelsPerByte == 4)
	{
		modnum = 0b11;
		byteInc = 2;
	}
	else if (numPixelsPerByte == 2)
	{
		modnum = 0b1111;
		byteInc = 4;
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
			for (int byteIndex = 0; byteIndex < 8; byteIndex += byteInc)
			{
				int curColor = (static_cast<int>(curByte) >> ((8 - byteInc) - byteIndex)) & modnum;
				if (indexX * numPixelsPerByte + (byteIndex / byteInc) < outImage.width)
				{
					outImage.pixel_data[static_cast<size_t>(indexY * outImage.width + indexX * numPixelsPerByte + (byteIndex / byteInc))] = static_cast<unsigned char>(curColor);
				}
			}
			curPos++;
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
			return -4; // Only dungeons have missing offsets
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

int UltimaVResource::LoadPathFile()
{
	const std::string file_name = "BRITISH.PTH";
	std::filesystem::path file_path = GAME_DIRECTORY / file_name;
	if (!std::filesystem::exists(file_path))
	{
		return -1;
	}
	std::uintmax_t file_size = std::filesystem::file_size(file_path);
	std::ifstream file(file_path, std::ios::binary);
	m_PathFileData.resize(file_size);
	file.read(reinterpret_cast<char*>(m_PathFileData.data()), static_cast<std::streamsize>(file_size));
	file.close();

	return 0;
}

int UltimaVResource::LoadBitFiles()
{
	const std::vector<std::string> file_names = { "BRITISH.BIT", "TITLE.BIT", "WD.BIT" };
	m_BitFileData.resize(file_names.size());
	
	int array_pos = 0;

	for (std::string strFile : file_names)
	{
		std::filesystem::path file_path = GAME_DIRECTORY / strFile;
		if (!std::filesystem::exists(file_path))
		{
			return -1;
		}
		std::uintmax_t file_size = std::filesystem::file_size(file_path);
		std::vector<unsigned char> buffer(file_size);
		std::ifstream file(file_path, std::ios::binary);
		file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
		file.close();

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
			std::filesystem::path file_path = GAME_DIRECTORY / (file_names[index] + file_extensions[ext_index]);
			if (!std::filesystem::exists(file_path))
			{
				return -1;
			}
			std::uintmax_t file_size = std::filesystem::file_size(file_path);
			std::vector<unsigned char> buffer(file_size);
			std::ifstream file(file_path, std::ios::binary);
			file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
			file.close();

			m_CharacterSetsData[index][ext_index].resize(NUM_CHARACTERS);

			if (0 != ParseCharacterFile(m_CharacterSetsData[index][ext_index], buffer, WIDTH[ext_index], HEIGHT[ext_index]))
			{
				return -2;
			}
		}
	}
	return 0;
}

int UltimaVResource::Load16Images()
{
	const std::string strExt(".16");
	const std::vector<std::string> file_names = { "CREATE", "END1", "END2", "ENDSC", "STARTSC",
			"STORY1", "STORY2", "STORY3", "STORY4", "STORY5", "STORY6", "TEXT", "ULTIMA" };

	m_Image16FileData.resize(file_names.size());

	for (size_t index = 0; index < file_names.size(); index++)
	{
		std::filesystem::path file_path = GAME_DIRECTORY / (file_names[index] + strExt);
		if (!std::filesystem::exists(file_path))
		{
			return -1;
		}
		std::uintmax_t file_size = std::filesystem::file_size(file_path);
		std::vector<unsigned char> buffer(file_size);
		std::ifstream file(file_path, std::ios::binary);
		file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
		file.close();

		std::vector<unsigned char> buffer_lzw;
		if (!LzwDecompressor::Extract(buffer, buffer_lzw))
		{
			return -2;
		}
		auto& curVec = m_Image16FileData[index];

		if (0 != Parse16File(curVec, buffer_lzw, 2))
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

int UltimaVResource::LoadStory(std::vector<unsigned char> &data_buffer)
{
	const size_t NUM_DATA = 0x2a; // 42
	const size_t NUM_STORIES = 21;
	const size_t X_LEFT_OFFSET = 0x2fa8;
	const size_t X_RIGHT_OFFSET = 0x2fd2;
	const size_t STORY_OFFSET = 0x3026;
	//const size_t INTRO_OFFSET_IN_DATA_1 = 0x2f41;
	//const size_t INTRO_OFFSET_IN_DATA_2 = 0x2f6f;
	const size_t STORY_IMAGE_INDEX = 0x30a8;
	const size_t STORY_NUMBER = 0x30be;
	const size_t STORY_PIC_X = 0x30d4;
	const size_t STORY_PIC_Y = 0x30ea;
	const size_t STORY_PIC_SECONDARY = 0x3100;

	std::string strStoryFile("STORY.DAT");
	std::filesystem::path file_path = GAME_DIRECTORY / strStoryFile;
	if (!std::filesystem::exists(file_path))
	{
		return -1;
	}
	std::uintmax_t file_size = std::filesystem::file_size(file_path);
	std::vector<unsigned char> buffer(file_size);
	std::ifstream file(file_path, std::ios::binary);
	file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
	file.close();

	for (size_t index = 0; index < NUM_DATA; index++)
	{
		m_data.story_data[index].text_left_pos = data_buffer[X_LEFT_OFFSET + index];
		//m_data.story_data[index].text_right_pos = data_buffer[index];
		size_t temppos = X_RIGHT_OFFSET + index * 2;
		m_data.story_data[index].text_right_pos = ReadInt16(data_buffer.begin(), temppos);
	}

	for (size_t index = 0; index < NUM_STORIES; index++)
	{
		size_t temppos = STORY_OFFSET + index * 2;
		m_data.story_text[index].text_offset = ReadInt16(data_buffer.begin(), temppos);
		if (m_data.story_text[index].text_offset >= buffer.size())
		{
			return -2;
		}
		// Action 3 overrides everything
		if (m_data.story_text[index].text_offset == 0)
		{
			LoadStoryText(buffer, m_data.story_text[index].text_offset, m_data.story_text[index].text);
		}
		m_data.story_text[index].image_index = data_buffer[STORY_IMAGE_INDEX + index];
		m_data.story_text[index].story_number = data_buffer[STORY_NUMBER + index];
		m_data.story_text[index].picture_x = data_buffer[STORY_PIC_X + index];
		m_data.story_text[index].picture_y = data_buffer[STORY_PIC_Y + index];
		m_data.story_text[index].action = data_buffer[STORY_PIC_SECONDARY + index];
	}

	return 0;
}

int UltimaVResource::LoadDataOvl()
{
	std::string strStoryFile("DATA.OVL");
	std::filesystem::path file_path = GAME_DIRECTORY / strStoryFile;
	if (!std::filesystem::exists(file_path))
	{
		return -1;
	}
	std::uintmax_t file_size = std::filesystem::file_size(file_path);
	std::vector<unsigned char> buffer(file_size);
	std::ifstream file(file_path, std::ios::binary);
	file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
	file.close();
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
	return 0;
}

