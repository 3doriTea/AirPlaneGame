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
#include <d3d11.h>
#include <DirectXMath.h>
#include "Texture2D.h"
#include "DirectX11Draw.h"
#include "IncludingWindows.h"
#include "Vector2.h"
#include "IShader.h"
#include "CameraSystem.h"
#include "MTStringUtility.h"
using Microsoft::WRL::ComPtr;

namespace mtgb
{

	struct AABB
	{
		Vector3 min, max;
	};

	struct TerrainVertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 uv;
	};

	struct TerrainConstantBuffer
	{
		DirectX::XMMATRIX matWVP;
		DirectX::XMMATRIX matNormalTrans;
		DirectX::XMFLOAT4 lightDir;
	};
	
	template<typename StageDataBit>
	class TerrainReader : IShader
	{
	public:
		TerrainReader();
		void ReadTerrain(const char* fileName);
		void Initialize() override;
		float GetHeightAt(float x, float z) const;
		void GenerateQuadtreeHeightMap();
		void GenerateTerrainAABBs(std::vector<Collider*>* _aabbs);
		void TestDraw();
		/// <summary>
		/// <para> ワールド座標からそれに対応する terrainのセルの番号を返す </para>
		/// <para> terrainの真ん中を(0,0)、原点とする </para>
		/// </summary>
		/// <param name="_point">セルの番号</param>
		/// <returns></returns>
		int WorldToCellIndex(float _point) const;
		/// <summary>
		/// <para> terrainのセルの番号からワールド座標を返す </para>
		/// </summary>
		/// <param name="_cellIndex"></param>
		/// <returns></returns>
		float CellIndexToWorld(int _cellIndex) const;
		
		void GenerateTerrainMesh();
		
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

		void CreateTextureMipmap(const std::wstring& _fileName);
		void DrawTerran() const;

		/// <summary>
		/// ステージの範囲外と範囲内の境界となるコライダーを作成する
		/// </summary>
		void GenerateStageBoundaryCollider();
	private:
		// メッシュのデータ
		std::vector<TerrainVertex> vertices_;
		std::vector<DWORD> indices_;

		// Comオブジェクト
		// バッファ
		/*ComPtr<ID3D11Buffer> pVertexBuffer_;
		ComPtr<ID3D11Buffer> pIndexBuffer_;*/

		// テクスチャ
		//ComPtr<ID3D11Texture2D> pTexture_;
		Texture2D pTexture_;

		// ミップマップレベル
		int mipLevels_;

		void GenerateNormals();
		Vector3 CalculateNormal(int _x, int _z) const;

		int width, height;
		float heightScale;
		float widthScale;
		std::vector<StageDataBit> stageBuffer;
		
		std::vector<Collider*> aabbs;
		std::vector<std::vector<float>> quadtreeHeightMap;
		int divisions; //分割回数
		int cellNum;

		// ステージの最高高度
		float highestHeight;
		// 天井をステージの最高高度からどれだけ上に配置するかのオフセット
		float ceilingOffset;

		Transform* pTransform;
		FBXModelHandle hModelCollider_;


	};

	using TerrainReader8 = TerrainReader<uint8_t>;
	using TerrainReader16 = TerrainReader<uint16_t>;

	template<typename StageDataBit>
	inline TerrainReader<StageDataBit>::TerrainReader()
		: width{513}
		, height{513}
		, heightScale{50.0f}
		, widthScale{300.0f}
		, divisions{3}
		, highestHeight{0.0f}
		, ceilingOffset{100.0f}
	{
		stageBuffer.resize(width * height);
		hModelCollider_ = Fbx::Load("Model/BoxCollider.fbx");
		EntityId id = Game::CreateEntity();
		pTransform = &(Transform::Get(id));
		pTransform->scale = { 1,1,1 };
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

		
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::Initialize()
	{
		CreateTextureMipmap(ToWString(std::string{ "Image/Dirt.png" }));
		ReadTerrain("terrain.raw");
		GenerateQuadtreeHeightMap();
		GenerateStageBoundaryCollider();
		GenerateTerrainAABBs(&aabbs);
		GenerateTerrainMesh();

		IShader::Initialize();
		
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
				float height = normalized * heightScale;
				quadtreeHeightMap[y][x] = height;

				if (highestHeight < height)
				{
					highestHeight = height;
				}
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

				pCollider->isStatic_ = true;
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
		DirectX11Draw::SetIsWriteToDepthBuffer(true);
		DirectX11Draw::SetBlendMode(BlendMode::Default);
		Draw::CheckSetShader(ShaderType::Terrain);
		
		ID3D11ShaderResourceView* pSRV = pTexture_.GetShaderResourceView();
		ID3D11SamplerState* pSampler = pTexture_.GetSamplerState();

		IShader::Draw<TerrainConstantBuffer, TerrainVertex>(
			// コンスタントバッファ書き込み
			[&](TerrainConstantBuffer* _pConstantBuffer)
			{
				const CameraSystem& camera = Game::System<CameraSystem>();

				Matrix4x4 world;
				pTransform->GenerateWorldMatrix(&world);

				Matrix4x4 view, proj;
				camera.GetViewMatrix(&view);
				camera.GetProjMatrix(&proj);

				_pConstantBuffer->matWVP = DirectX::XMMatrixTranspose(world * view * proj);
				_pConstantBuffer->matNormalTrans = DirectX::XMMatrixTranspose(pTransform->matrixRotate_ * DirectX::XMMatrixInverse(nullptr, pTransform->matrixScale_));
				_pConstantBuffer->lightDir = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
			},
			[&](ID3D11DeviceContext* _pContext)
			{

				_pContext->PSSetShaderResources(0, 1, &pSRV);
				_pContext->PSSetSamplers(0, 1, &pSampler);
			}, static_cast<int>(indices_.size()));

		/*for (int z = 0; z < cellNum; z++)
		{
			for (int x = 0; x < cellNum; x++)
			{
				
				aabbs[z * cellNum + x]->Draw();
			}
		}*/
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

		// 境界チェック（頂点数に合わせて0からcellNumまで）
		if (cellIndex < 0 || cellIndex > cellNum)
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
		// 境界チェック（頂点数に合わせて0からcellNumまで）
		if (_cellIndex < 0 || _cellIndex > cellNum)
		{
			return 0.0f;
		}
		// 原点をグリッドの中央とするオフセット
		int centerOffset = cellNum / 2;

		// 中心基準の座標系にしてからワールド座標系に変換
		return (_cellIndex - centerOffset) * widthScale;
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::GenerateTerrainMesh()
	{
		vertices_.clear();
		indices_.clear();

		// 頂点生成のみ
		for (int z = 0; z <= cellNum; z++)
		{
			for (int x = 0; x <= cellNum; x++)
			{
				TerrainVertex vertex;

				// 位置
				vertex.position.x = CellIndexToWorld(x);
				vertex.position.y = quadtreeHeightMap[z][x];  
				vertex.position.z = CellIndexToWorld(z);

				// UV座標 (0.0～1.0)
				vertex.uv.x = static_cast<float>(x) / cellNum;
				vertex.uv.y = static_cast<float>(z) / cellNum;
				
				vertices_.push_back(vertex);
			}
		}

		// インデックス生成（ポリゴン単位）
		for (int z = 0; z < cellNum; z++)  // cellNum-1まで（ポリゴン数）
		{
			for (int x = 0; x < cellNum; x++)  // cellNum-1まで（ポリゴン数）
			{
				// 四角形の4つの頂点インデックス
				int topLeft = z * (cellNum + 1) + x;
				int topRight = topLeft + 1;
				int bottomLeft = (z + 1) * (cellNum + 1) + x;
				int bottomRight = bottomLeft + 1;

				// 一つ目の三角形 (時計回り)
				indices_.push_back(topLeft);
				indices_.push_back(topRight);
				indices_.push_back(bottomLeft);

				// 二つ目の三角形 (時計回り)
				indices_.push_back(topRight);
				indices_.push_back(bottomRight);
				indices_.push_back(bottomLeft);
			}
		}

		// 法線計算
		GenerateNormals();
		
		
		
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::InitializeVertexBuffer(ID3D11Device* _pDevice)
	{
		D3D11_BUFFER_DESC bufferDesc =
		{
			.ByteWidth = static_cast<UINT>(sizeof(TerrainVertex) * vertices_.size()),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = 0,
		};

		D3D11_SUBRESOURCE_DATA initData =
		{
			.pSysMem = vertices_.data()
		};

		HRESULT hResult = _pDevice->CreateBuffer(
			&bufferDesc, &initData, &pVertexBuffer_);

		massert(SUCCEEDED(hResult) && "頂点バッファの作成に失敗");
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::InitializeIndexBuffer(ID3D11Device* _pDevice)
	{
		D3D11_BUFFER_DESC bufferDesc =
		{
			.ByteWidth = static_cast<UINT>(sizeof(DWORD) * indices_.size()),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = 0
		};

		D3D11_SUBRESOURCE_DATA initData =
		{
			.pSysMem = indices_.data()
		};

		HRESULT hResult = _pDevice->CreateBuffer(
			&bufferDesc, &initData, &pIndexBuffer_
		);

		massert(SUCCEEDED(hResult) && "インデックスバッファの作成に失敗");
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::InitializeConstantBuffer(ID3D11Device* _pDevice)
	{
		D3D11_BUFFER_DESC bufferDesc =
		{
			.ByteWidth = sizeof(TerrainConstantBuffer),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
		};

		HRESULT hResult = _pDevice->CreateBuffer(
			&bufferDesc,
			nullptr,
			pConstantBuffer_.ReleaseAndGetAddressOf()
		);

		massert(SUCCEEDED(hResult)
			&& "コンスタントバッファの作成に失敗 @FbxParts::InitializeConstantBuffer");
	}

	

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::CreateTextureMipmap(const std::wstring& _fileName)
	{
		pTexture_.Load(_fileName);
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::DrawTerran() const
	{
		
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::GenerateStageBoundaryCollider()
	{
		float stageMin = CellIndexToWorld(0);
		float stageMax = CellIndexToWorld(cellNum - 1);

		
		float wallHeight = highestHeight + ceilingOffset;
		float wallThickness = widthScale;
		
		// +Z方向に位置する壁
		{
			EntityId id = Game::CreateEntity();
			Collider* pCollider = &(Game::System<ColliderCP>().Get(id, Collider::ColliderTag::STAGE_BOUNDARY));
			pCollider->type_ = Collider::TYPE_AABB;

			Vector3 center =
			{
				(stageMin + stageMax) / 2.0f,
				wallHeight / 2.0f,
				stageMax + (wallThickness / 2.0f)
			};

			Vector3 extents =
			{
				(stageMax - stageMin) / 2.0f,
				wallHeight / 2.0f,
				wallThickness / 2.0f
			};

			pCollider->isStatic_ = true;
			pCollider->SetCenter(center);
			pCollider->SetExtents(extents);
			aabbs.push_back(pCollider);
		}

		// -Z方向に位置する壁
		{
			EntityId id = Game::CreateEntity();
			Collider* pCollider = &(Game::System<ColliderCP>().Get(id, Collider::ColliderTag::STAGE_BOUNDARY));
			pCollider->type_ = Collider::TYPE_AABB;

			Vector3 center =
			{
				(stageMin + stageMax) / 2.0f,
				wallHeight / 2.0f,
				stageMin - (wallThickness / 2.0f)
			};

			Vector3 extents =
			{
				(stageMax - stageMin) / 2.0f,
				wallHeight / 2.0f,
				wallThickness / 2.0f
			};

			pCollider->isStatic_ = true;
			pCollider->SetCenter(center);
			pCollider->SetExtents(extents);
			aabbs.push_back(pCollider);
		}

		// +X方向に位置する壁
		{
			EntityId id = Game::CreateEntity();
			Collider* pCollider = &(Game::System<ColliderCP>().Get(id, Collider::ColliderTag::STAGE_BOUNDARY));
			pCollider->type_ = Collider::TYPE_AABB;

			Vector3 center =
			{
				stageMax + (wallThickness / 2.0f),
				wallHeight / 2.0f,
				(stageMin + stageMax) / 2.0f
			};

			Vector3 extents =
			{
				wallThickness / 2.0f,
				wallHeight / 2.0f,
				(stageMax - stageMin) / 2.0f
			};

			pCollider->isStatic_ = true;
			pCollider->SetCenter(center);
			pCollider->SetExtents(extents);
			aabbs.push_back(pCollider);
		}

		// -X方向に位置する壁
		{
			EntityId id = Game::CreateEntity();
			Collider* pCollider = &(Game::System<ColliderCP>().Get(id, Collider::ColliderTag::STAGE_BOUNDARY));
			pCollider->type_ = Collider::TYPE_AABB;

			Vector3 center =
			{
				stageMin - (wallThickness / 2.0f),
				wallHeight / 2.0f,
				(stageMin + stageMax) / 2.0f
			};

			Vector3 extents =
			{
				wallThickness / 2.0f,
				wallHeight / 2.0f,
				(stageMax - stageMin) / 2.0f
			};

			pCollider->isStatic_ = true;
			pCollider->SetCenter(center);
			pCollider->SetExtents(extents);
			aabbs.push_back(pCollider);
		}

		// +Y方向に位置する壁(天井)
		{
			EntityId id = Game::CreateEntity();
			Collider* pCollider = &(Game::System<ColliderCP>().Get(id, Collider::ColliderTag::STAGE_BOUNDARY));
			pCollider->type_ = Collider::TYPE_AABB;

			Vector3 center =
			{
				(stageMin + stageMax) / 2.0f,
				wallHeight + (wallThickness / 2.0f),
				(stageMin + stageMax) / 2.0f
			};

			Vector3 extents =
			{
				(stageMax - stageMin) / 2.0f,
				wallThickness / 2.0f,
				(stageMax - stageMin) / 2.0f
			};

			pCollider->isStatic_ = true;
			pCollider->SetCenter(center);
			pCollider->SetExtents(extents);
			aabbs.push_back(pCollider);
		}
	}

	template<typename StageDataBit>
	inline void TerrainReader<StageDataBit>::GenerateNormals()
	{
		for (int z = 0; z <= cellNum; z++)
		{
			for (int x = 0; x <= cellNum; x++)
			{
				int index = z * (cellNum + 1) + x;
				vertices_[index].normal = CalculateNormal(x, z);
			}
		}
	}

	template<typename StageDataBit>
	inline Vector3 TerrainReader<StageDataBit>::CalculateNormal(int _x, int _z) const
	{
		Vector3 normal = Vector3::Zero();

		// 周囲の高さから法線を計算
		// _x,_zが端の場合に配列外参照しないように境界チェックを行う
		float heightLeft = (_x > 0) ? quadtreeHeightMap[_z][_x - 1] : quadtreeHeightMap[_z][_x];
		float heightRight = (_x < cellNum) ? quadtreeHeightMap[_z][_x + 1] : quadtreeHeightMap[_z][_x];
		float heightDown = (_z > 0) ? quadtreeHeightMap[_z - 1][_x] : quadtreeHeightMap[_z][_x];
		float heightUp = (_z < cellNum) ? quadtreeHeightMap[_z + 1][_x] : quadtreeHeightMap[_z][_x];

		// 左右の高低差
		normal.x = heightLeft - heightRight;
		
		// 法線にy方向を向かせる
		normal.y = 2.0f * widthScale;

		// 上下の高低差
		normal.z = heightDown - heightUp;

		return Vector3::Normalize(normal);
	}


}