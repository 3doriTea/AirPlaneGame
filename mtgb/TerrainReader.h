#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "MTAssert.h"
#include <limits>
#include "MTImGui.h"
namespace mtgb
{

	

	template<typename StageDataBit>
	class TerrainReader
	{
	public:
		TerrainReader();
		void ReadTerrain(const char* fileName);
		void TestDraw();
		int width, height;
		float heightScale;
		float widthScale;
		std::vector<StageDataBit> stageBuffer;
		std::vector<std::vector<float>> stageData;


	};

	using TerrainReader8 = TerrainReader<uint8_t>;
	using TerrainReader16 = TerrainReader<uint16_t>;

	template<typename StageDataBit>
	inline TerrainReader<StageDataBit>::TerrainReader()
		: width{513}
		, height{513}
		, heightScale{50.0f}
		, widthScale{5.0f}
	{
		stageBuffer.resize(width * height);
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::ReadTerrain(const char* fileName)
	{
		// ファイルをバイナリファイルとして読み込む
		std::ifstream file(fileName, std::ios::binary);
		massert(file && "バイナリファイルの読み込みに失敗");

		file.read(reinterpret_cast<char*>(stageBuffer.data()), stageBuffer.size() * sizeof(StageDataBit));
		massert(file && "16bitバイナリファイルの読み込みに失敗");

		stageData.resize(height, std::vector<float>(width));
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				StageDataBit value = stageBuffer[y * width + x];
				float normalized = static_cast<float>(value) / (std::numeric_limits<StageDataBit>::max)();
				stageData[y][x] = normalized * heightScale;
			}
		}
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::TestDraw()
	{
		//for (int y = 0; y < height;y++)
		//{
		//	for (int x = 0; x < width;x++)
		//	{
		//		Vector3 start = { widthScale * x ,stageData[y][x] ,widthScale * y };
		//		Vector3 end = { widthScale * x, 0, widthScale * y };

		//		MTImGui::Instance().DrawLine(start, end, 3.0f);
		//		//stageData[y][x]
		//	}
		//}
	}

	

}