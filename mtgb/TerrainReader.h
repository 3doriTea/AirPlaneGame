#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "MTAssert.h"
#include <limits>
#include "MTImGui.h"
#include "Vector3.h"
#include <DirectXCollision.h>
#include "Draw.h"
#include "Transform.h"
#include "Fbx.h"
#include "ColliderCP.h"
#include "Collider.h"
namespace mtgb
{

	struct AABB
	{
		Vector3 min, max;
	};

	

	template<typename StageDataBit>
	class TerrainReader
	{
	public:
		TerrainReader();
		void ReadTerrain(const char* fileName);
		float GetHeightAt(float x, float z) const;
		void GenerateQuadtreeHeightMap();
		void GenerateTerrainAABBs(std::vector<Collider*>* _aabbs);
		void TestDraw();
		int WorldToCellIndex(float _point) const;
		float CellIndexToWorld(int _cellIndex) const;
		

		int width, height;
		float heightScale;
		float widthScale;
		std::vector<StageDataBit> stageBuffer;
		
		std::vector<Collider*> aabbs;
		std::vector<std::vector<float>> quadtreeHeightMap;
		int divisions; //分割回数
		int cellNum;

		Transform* pTransform;
		FBXModelHandle hModelCollider_;
	};

	using TerrainReader8 = TerrainReader<uint8_t>;
	using TerrainReader16 = TerrainReader<uint16_t>;

	template<typename StageDataBit>
	inline TerrainReader<StageDataBit>::TerrainReader()
		: width{513}
		, height{513}
		, heightScale{200.0f}
		, widthScale{5.0f}
		, divisions{3}
	{
		stageBuffer.resize(width * height);
		hModelCollider_ = Fbx::Load("Model/BoxCollider.fbx");
		pTransform = new Transform();

	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::ReadTerrain(const char* fileName)
	{
		// ファイルをバイナリファイルとして読み込む
		std::ifstream file(fileName, std::ios::binary);
		massert(file && "バイナリファイルの読み込みに失敗");

		file.read(reinterpret_cast<char*>(stageBuffer.data()), stageBuffer.size() * sizeof(StageDataBit));
		massert(file && "16bitバイナリファイルの読み込みに失敗");

		/*for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				StageDataBit value = stageBuffer[y * width + x];
				float normalized = static_cast<float>(value) / (std::numeric_limits<StageDataBit>::max)();
				stageData[y][x] = normalized * heightScale;
			}
		}*/

		GenerateQuadtreeHeightMap();
	}

	template<typename StageDataBit>
	inline float TerrainReader<StageDataBit>::GetHeightAt(float x, float z) const
	{
		float mapX = WorldToCellIndex(x);
		float mapZ = WorldToCellIndex(z);

		// 境界チェック
		if (mapX < 0 || mapZ < 0)
		{
			return 0.0f;
		}

		
		int x0 = static_cast<int>(mapX);
		int z0 = static_cast<int>(mapZ);
		int x1 = x0 + 1;
		int z1 = z0 + 1;

		// セル内のローカル座標
		float localX = mapX - x0;
		float localY = mapZ - z0;

		// 左上
		float topLeft = quadtreeHeightMap[z0][x0];
		// 右上
		float topRight = quadtreeHeightMap[z0][x1];
		// 左下
		float bottomLeft = quadtreeHeightMap[z1][x0];
		// 右下
		float bottomRight = quadtreeHeightMap[z1][x1];

		// x方向で線形補間
		// (1-t) * A + t * B
		float top = (1 - localX) * topLeft + topRight * localX;
		float bottom = (1 - localX) * bottomLeft + bottomRight * localX;

		// y方向で線形補間
		return (1 - localY) * top + bottom * localY;
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::GenerateQuadtreeHeightMap()
	{
		// 一辺のセル数
		cellNum = static_cast<int>(std::pow(2, divisions));
		//aabbs.resize(cellNum * cellNum);
		// 元のデータから値を取得する間隔
		// unityの terrain Dataにはセル数+1の頂点数が入っているので-1する
		float samplingStep = static_cast<float>(width - 1) / (cellNum);

		quadtreeHeightMap.resize(cellNum + 1, std::vector<float>(cellNum + 1));

		for (int y = 0; y < cellNum + 1; y++)
		{
			for (int x = 0; x < cellNum + 1; x++)
			{
				// 元のデータからサンプリング
				int sourceX = static_cast<int>(x * samplingStep);
				int sourceY = static_cast<int>(y * samplingStep);

				// 境界のチェック
				sourceX = (std::min)(sourceX, width - 1);
				sourceY = (std::min)(sourceY, height - 1);

				StageDataBit value = stageBuffer[sourceY * width + sourceX];
				float normalized = static_cast<float>(value) / (std::numeric_limits<StageDataBit>::max)();
				quadtreeHeightMap[y][x] = normalized * heightScale;
			}
		}
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::GenerateTerrainAABBs(std::vector<Collider*>* _aabbs)
	{
		for (int z = 0; z < cellNum; z++)
		{
			for (int x = 0; x < cellNum; x++)
			{
				// 左上
				float topLeft = quadtreeHeightMap[z][x];
				// 右上
				float topRight = quadtreeHeightMap[z][x + 1];
				// 左下
				float bottomLeft = quadtreeHeightMap[z + 1][x];
				// 右下
				float bottomRight = quadtreeHeightMap[z + 1][x + 1];

				//// セルの最小、最高高度
				//// セルの中から補間はせずに
				//float minHeight = (std::min)({ topLeft,topRight,bottomLeft,bottomRight });
				float maxHeight = (std::max)({ topLeft,topRight,bottomLeft,bottomRight });

				float height = maxHeight;
				//float height = GetHeightAt(x, z);
				// ワールド座標系に変換
				
				Vector3 cellWorldPos = { CellIndexToWorld(x), height, CellIndexToWorld(z) };

				Vector3 extents = { widthScale / 2.0f, height / 2.0f,widthScale / 2.0f };
				Vector3 center = cellWorldPos - extents;
				//_aabbs->emplace_back(center, extents);
				
				EntityId terrainCellId = Game::CreateEntity();
				// 静的なコライダー
				Collider* pCollider = &(Game::System<ColliderCP>().Get(terrainCellId,Collider::ColliderTag::STAGE));

				pCollider->type_ = Collider::TYPE_AABB;
				pCollider->SetCenter(center);
				pCollider->SetExtents(extents);
				_aabbs->push_back(pCollider);
			}
		}
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::TestDraw()
	{
		for (int z = 0; z < cellNum; z++)
		{
			for (int x = 0; x < cellNum; x++)
			{
				/*DirectX::BoundingBox& box = aabbs[z * cellNum + x];
				pTransform->position = DirectX::XMLoadFloat3(&box.Center);
				pTransform->scale = DirectX::XMLoadFloat3(&box.Extents) * 2;

				pTransform->Compute();
				Draw::FBXModel(hModelCollider_, *pTransform,0,ShaderType::Debug3D);*/
				aabbs[z * cellNum + x]->Draw();
			}
		}
	}

	template<typename StageDataBit>
	inline int TerrainReader<StageDataBit>::WorldToCellIndex(float _point) const
	{
		// セル単位に変換
		// 常に小さい方,左側へ丸めたいのでfloorf
		int cell = static_cast<int>(std::floorf(_point / widthScale));

		// セル変換した値の原点をグリッドの中央とするオフセット
		// cellNumが8なら頂点数は [0 1 2 3 4 5 6 7 8] で4が中央になる
		// [-4 -3 -2 -1 0 1 2 3 4]をオフセット分ずらす
		int centerOffset = cellNum / 2;

		// セル単位に変換した値をオフセット分ずらす
		int cellIndex = cell + centerOffset;

		// 境界チェック
		if (cellIndex < 0 || cellIndex >= cellNum)
		{
			return -1;
		}
		else
		{
			return cellIndex;
		}
	}

	template<typename StageDataBit>
	inline float TerrainReader<StageDataBit>::CellIndexToWorld(int _cellIndex) const
	{
		// 境界チェック
		if (_cellIndex < 0 || _cellIndex >= cellNum)
		{
			return 0.0f;
		}
		// 原点をグリッドの中央とするオフセット
		int centerOffset = cellNum / 2;

		// 中心基準の座標系にしてからワールド座標系に変換
		return (_cellIndex - centerOffset) * widthScale;
	}

}