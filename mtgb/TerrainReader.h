//#pragma once
//
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <cstdint>
//#include "MTAssert.h"
//#include <limits>
//#include "MTImGui.h"
//namespace mtgb
//{
//
//	
//
//	template<typename StageDataBit>
//	class TerrainReader
//	{
//	public:
//		TerrainReader();
//		void ReadTerrain(const char* fileName);
//		float GetHeightAt(float x, float z) const;
//		void GenerateQuadtreeHeightMap();
//		void TestDraw();
//		int width, height;
//		float heightScale;
//		float widthScale;
//		std::vector<StageDataBit> stageBuffer;
//		std::vector<std::vector<float>> stageData;
//
//		std::vector<std::vector<float>> quadtreeHeightMap;
//		int divisions; //分割回数
//		int cellNum;
//	};
//
//	using TerrainReader8 = TerrainReader<uint8_t>;
//	using TerrainReader16 = TerrainReader<uint16_t>;
//
//	template<typename StageDataBit>
//	inline TerrainReader<StageDataBit>::TerrainReader()
//		: width{513}
//		, height{513}
//		, heightScale{50.0f}
//		, widthScale{5.0f}
//	{
//		stageBuffer.resize(width * height);
//	}
//
//	template<typename StageDataBit>
//	inline void TerrainReader<StageDataBit>::ReadTerrain(const char* fileName)
//	{
//		// ファイルをバイナリファイルとして読み込む
//		std::ifstream file(fileName, std::ios::binary);
//		massert(file && "バイナリファイルの読み込みに失敗");
//
//		file.read(reinterpret_cast<char*>(stageBuffer.data()), stageBuffer.size() * sizeof(StageDataBit));
//		massert(file && "16bitバイナリファイルの読み込みに失敗");
//
//		stageData.resize(height, std::vector<float>(width));
//		/*for (int y = 0; y < height; y++)
//		{
//			for (int x = 0; x < width; x++)
//			{
//				StageDataBit value = stageBuffer[y * width + x];
//				float normalized = static_cast<float>(value) / (std::numeric_limits<StageDataBit>::max)();
//				stageData[y][x] = normalized * heightScale;
//			}
//		}*/
//
//		GenerateQuadtreeHeightMap();
//	}
//
//	template<typename StageDataBit>
//	inline float TerrainReader<StageDataBit>::GetHeightAt(float x, float z) const
//	{
//		float mapX = (x / widthScale) + (cellNum) * 0.5f;
//		float mapZ = (z / widthScale) + (cellNum) * 0.5f;
//
//		// 境界チェック
//		if (mapX < 0 || mapX >= cellNum || mapZ < 0 || mapZ >= cellNum)
//		{
//			return 0.0f;
//		}
//
//		
//		int x0 = static_cast<int>(mapX);
//		int z0 = static_cast<int>(mapZ);
//		int x1 = x0 + 1;
//		int z1 = z0 + 1;
//
//		// セル内のローカル座標
//		float localX = mapX - x0;
//		float localY = mapZ - z0;
//
//		// 左上
//		float topLeft = quadtreeHeightMap[z0][x0];
//		// 右上
//		float topRight = quadtreeHeightMap[z0][x1];
//		// 左下
//		float bottomLeft = quadtreeHeightMap[z1][x0];
//		// 右下
//		float bottomRight = quadtreeHeightMap[z1][x1];
//
//		// x方向で線形補間
//		// (1-t) * A + t * B
//		float top = (1 - localX) * topLeft + topRight * localX;
//		float bottom = (1 - localX) * bottomLeft + bottomRight * localX;
//
//		// y方向で線形補間
//		return (1 - localY) * top + bottom * localY;
//	}
//
//	template<typename StageDataBit>
//	inline void TerrainReader<StageDataBit>::GenerateQuadtreeHeightMap()
//	{
//		// 一辺のセル数
//		cellNum = std::pow(2, divisions) + 1;
//
//		// 元のデータから値を取得する間隔
//		// unityの terrain Dataにはセル数+1の頂点数が入っているので-1する
//		float samplingStep = static_cast<float>(width - 1) / (cellNum);
//
//		quadtreeHeightMap.resize(cellNum * std::vector<float>(quadtreeHeightMap));
//
//		for (int y = 0; y < cellNum + 1; y++)
//		{
//			for (int x = 0; x < cellNum + 1; x++)
//			{
//				// 元のデータからサンプリング
//				int sourceX = static_cast<int>(x * samplingStep);
//				int sourceY = static_cast<int>(y * samplingStep);
//
//				// 境界のチェック
//				sourceX = (std::min)(sourceX, width - 1);
//				sourceY = (std::min)(sourceY, height - 1);
//
//				StageDataBit value = stageBuffer[sourceY * width + sourceX];
//				float normalized = static_cast<float>(value) / (std::numeric_limits<StageDataBit>::max)();
//				quadtreeHeightMap[y][x] = normalized * heightScale;
//			}
//		}
//	}
//
//	template<typename StageDataBit>
//	inline void TerrainReader<StageDataBit>::TestDraw()
//	{
//		
//	}
//
//	
//
//}