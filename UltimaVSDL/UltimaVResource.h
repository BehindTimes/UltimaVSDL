#pragma once
#include <vector>
#include <cstdint>

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
private:
	
	void Cleanup();

	int LoadBitFiles();
	int LoadPathFile();
	int Load16Images();
	int LoadCharacterSets();
	int ParseCharacterFile(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int width, int height);
	int ParseBitFile(std::vector<U5ImageData> &bit_file_data, std::vector<unsigned char> &data);
	int Parse16File(std::vector<U5ImageData>& bit_file_data, std::vector<unsigned char>& data, int numPixelsPerByte);
	uint32_t ReadInt16(std::vector<unsigned char> &data, size_t &curPos);
	uint32_t ReadInt32(std::vector<unsigned char> &data, size_t &curPos);
	int ReadOffsets(std::vector<unsigned char> &data, int offsetSize, int numOffsets, std::vector<size_t> &file_offsets, size_t &curPos);
	int ReadImage(std::vector<unsigned char> &data, size_t offset, int numPixelsPerByte, U5ImageData& outImage);
};

