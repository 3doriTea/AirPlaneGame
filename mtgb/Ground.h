#pragma once
#include "IShader.h"
#include <fbxsdk.h>

namespace mtgb
{
	class Ground : public IShader
	{
	public:
		struct Vertex
		{
			Vector3 position;  // 座標
			Vector3 normal;  // 法線
			Vector3 uv;  // uv座標
		};

		struct ConstantBuffer
		{
			Matrix4x4 g_matrixWorldViewProj;  // ワールド・ビュー・プロジェクション行列
			Matrix4x4 g_matrixNormalTrans;    // 回転行列と拡大行列の逆行列
			Matrix4x4 g_matrixWorld;  // ワールド行列
			Vector4 g_lightDirection;  // ライトの向き
			Vector4 g_diffuse;  // 光があたったときへの拡散反射光(マテリアル色)
			Vector4 g_ambient;  // 全体的な環境光 (光が当たらない場所にも明るく)
			Vector4 g_speculer;  // 鏡面反射 (Lambertの場合は0)
			Vector4 g_cameraPosition; // カメラの位置（ハイライトの計算に必要）
			FLOAT g_shininess;     // スペキュラの強さ
			BOOL g_isTexture;  // テクスチャの有無
		};

		Ground(FbxNode* _pParent);
		~Ground();

		void Initialize() override;
		void Release() override;

		void Draw();

	private:
		/// <summary>
		/// 頂点バッファの初期化
		/// </summary>
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// インデックスバッファの初期化
		/// </summary>
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// コンスタントバッファ
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

		/// <summary>
		/// インデックスの数
		/// </summary>
		/// <returns>インデックスの数</returns>
		inline uint32_t GetIndexCount() const
		{
			// インデックス数 = 1ポリゴン につき 3頂点 (3角化されているため)
			return polygonCount_ * 3;
		}
		/// <summary>
		/// ポリゴン数から頂点数を求める
		/// </summary>
		/// <param name="_polygonCount">ポリゴン数</param>
		/// <returns>頂点数</returns>
		inline uint32_t GetVertexCount(const int _polygonCount = 1) const
		{
			// 頂点数 = 1ポリゴン につき 3頂点
			return _polygonCount * 3;
		}

	private:
		uint32_t vertexCount_;  // 頂点数
		uint32_t polygonCount_;  // ポリゴン数
		uint32_t indexCount_;  // インデックス数
		uint32_t materialCount_;  // マテリアル数
		uint32_t polygonVertexCount_;  // ポリゴンの頂点数

		FbxNode* pNode_;  // Fbxノード情報
		FbxMesh* pMesh_;  // メッシュ

		Vertex* pVertexes_;  // 頂点情報
		DWORD* pIndexData_;  // インデックス情報

		ID3D11Buffer** ppIndexBuffer_;
	};
}

