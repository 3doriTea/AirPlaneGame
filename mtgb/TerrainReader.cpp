#include "TerrainReader.h"
#include <filesystem>
namespace mtgb
{
	//TerrainReader::TerrainReader()
	//	: width{ 50 }
	//	, height{ 50 }
	//	, heightScale{100.0f}
	//{
	//	stageBuffer.resize(width * height);
	//	
	//}
	//void mtgb::TerrainReader::ReadTerrain(const char* fileName)
	//{
	//	// ファイルをバイナリファイルとして読み込む
	//	std::ifstream file(fileName, std::ios::binary);
	//	massert(file && "バイナリファイルの読み込みに失敗");

	//	
	//		file.read(reinterpret_cast<char*>(stageBuffer.data()), stageBuffer.size() * sizeof(uint16_t));
	//		massert(file && "16bitバイナリファイルの読み込みに失敗");
	//	
	//		std::vector<uint8_t> tempBuffer(width * height);
	//		file.read(reinterpret_cast<char*>(tempBuffer.data()), tempBuffer.size());
	//		massert(file && "16bitバイナリファイルの読み込みに失敗");

	//	stageData.resize(height, std::vector<float>(width));
	//	

	//	for (int y = 0; y < height; y++)
	//	{
	//		for (int x = 0; x < width; x++)
	//		{
	//			uint16_t value = stageBuffer[y * width + x];
	//			float normalized = static_cast<float>(value) / UINT16_MAX;
	//			stageData[y][x] = normalized * heightScale;
	//		}
	//	}


	//	//auto a = stageData[0][0];
	//}
	
}
