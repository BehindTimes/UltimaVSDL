#include <filesystem>
#include <fstream>
#include <iostream>

#include "UltimaVResource.h"
#include "LzwDecompressor.h"

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

	return 0;
}

uint32_t UltimaVResource::ReadInt16(std::vector<unsigned char>& data, size_t& curPos)
{
	uint32_t retVal = (static_cast<uint32_t>(data[curPos + 1]) << 8) + static_cast<uint32_t>(data[curPos]);
	curPos += 2;
	return retVal;
}

uint32_t UltimaVResource::ReadInt32(std::vector<unsigned char>& data, size_t& curPos)
{
	uint32_t retVal = (static_cast<uint32_t>(data[curPos + 3]) << 24) + (static_cast<uint32_t>(data[curPos + 2]) << 16) +
		(static_cast<uint32_t>(data[curPos + 1]) << 8) + static_cast<uint32_t>(data[curPos]);
	curPos += 4;
	return retVal;
}

int UltimaVResource::ReadOffsets(std::vector<unsigned char> &data, int offsetSize, int numOffsets, std::vector<size_t> &file_offsets, size_t &curPos)
{
	if (data.size() < curPos + numOffsets * offsetSize)
	{
		return -1;
	}
	file_offsets.resize(numOffsets);
	for (int index = 0; index < numOffsets; index++)
	{
		uint32_t offset = 0;
		if (offsetSize == 2)
		{
			offset = ReadInt16(data, curPos);
		}
		else if (offsetSize == 4)
		{
			offset = ReadInt32(data, curPos);
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
	outImage.width = ReadInt16(data, offset);
	outImage.height = ReadInt16(data, offset);

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

	outImage.pixel_data.resize(outImage.width * outImage.height);
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
					outImage.pixel_data[indexY * outImage.width + indexX * numPixelsPerByte + (byteIndex / byteInc)] = static_cast<unsigned char>(curColor);
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
	uint32_t numImages = ReadInt16(data, curPos);
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
	uint32_t numImages = ReadInt16(data, curPos);
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
