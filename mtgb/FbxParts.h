#pragma once
#include "cmtgb.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "IShader.h"
#include "Texture2D.h"
#include <unordered_map>
#include <fbxsdk.h>


namespace fbxsdk
{/*
	class FbxMesh;
	class FbxSkin;*/
	class FbxCluster;
}

namespace mtgb
{
	class Fbx;
	class Transform;
	class DirectX11Draw;

	class FbxParts : public IShader
	{
		friend DirectX11Draw;
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

		/// <summary>
		/// マテリアル
		/// </summary>
		struct Material
		{
			uint32_t polygonCount;  // ポリゴン数
			Vector4 diffuse;  // 拡散反射光への反射強度
			Vector4 ambient;  // 環境光への反射強度
			Vector4 specular;  // 鏡面反射光
			float shininess;  // ハイライトの強さ
			Texture2D* pTexture;
		};

		/// <summary>
		/// ボーン (関節そのもの)
		/// </summary>
		struct Bone
		{
			// REF: https://help.autodesk.com/view/MAYACRE/JPN/?guid=GUID-36808BCC-ACF9-4A9E-B0D8-B8F509FEC0D5
			Matrix4x4 bindPose;  // 初期ポーズ時のボーン変換行列
			Matrix4x4 newPose;  // アニメーションで変化時のボーン変換行列
			Matrix4x4 diffPose;  // bindPoseに対する newPoseの変化量
		};

		/// <summary>
		/// ウェイト (関節同士の影響度合い)
		/// </summary>
		struct Weight
		{
			Vector3 posOrigin;  // もともとの頂点座標
			Vector3 normalOrigin;  // もともとの法線
			int* pBoneIndex;  // 関連するボーンのId
			float* pBoneWeight;  // ボーンの重み
		};

	public:
		FbxParts(FbxNode* _parent);
		~FbxParts();

		void Initialize() override;

		/// <summary>
		/// モデルを描画する
		/// </summary>
		/// <param name="_transform">座標系</param>
		void Draw(const Transform& _transform);
		/// <summary>
		/// ボーンありでモデルを描画する
		/// </summary>
		/// <param name="_transform">座標系</param>
		/// <param name="_time">アニメーションフレーム</param>
		void DrawSkinAnimation(const Transform& _transform, FbxTime _time);
		/// <summary>
		/// ボーンありでモデルを描画する
		/// </summary>
		/// <param name="_takeName"></param>
		/// <param name="_transform">座標系</param>
		/// <param name="_time">フレーム</param>
		void DrawSkinAnimation(const std::string& _takeName, const Transform& _transform, FbxTime _time);
		/// <summary>
		/// ボーン無しでモデルを描画する
		/// </summary>
		/// <param name="_transform">座標系</param>
		/// <param name="_time">フレーム</param>
		void DrawMeshAnimation(const Transform& _transform, FbxTime _time);

		/// <summary>
		/// 試しにボーンのアニメーション無しのときの座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePosition(const std::string& _boneName, Vector3* _pPosition);
		/// <summary>
		/// 試しにボーンのアニメーション中の座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePositionAtNow(const std::string& _boneName, Vector3* _pPosition);

		/// <summary>
		/// Fbxのスキンを取得
		/// </summary>
		/// <returns>スキンのポインタ</returns>
		FbxSkin* GetSkin() { return pSkin_; }

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
		/// コンスタントバッファの初期化
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

		/// <summary>
		/// マテリアルの初期化
		/// </summary>
		void InitializeMaterial();
		/// <summary>
		/// テクスチャの初期化
		/// </summary>
		void InitializeTexture(FbxSurfaceMaterial* _pMaterial, const DWORD _i);
		/// <summary>
		/// 骨情報の初期化
		/// </summary>
		void InitializeSkelton();

	private:
		uint32_t vertexCount_;  // 頂点数
		uint32_t polygonCount_;  // ポリゴン数
		uint32_t indexCount_;  // インデックス数
		uint32_t materialCount_;  // マテリアル数
		uint32_t polygonVertexCount_;  // ポリゴンの頂点数

		FbxNode* pNode_;  // Fbxノード情報
		Material* pMaterial_;  // マテリアル
		FbxMesh* pMesh_;  // メッシュ
		FbxSkin* pSkin_;  // スキンメッシュ情報 (スキンメッシュアニメーションのデータ)
		FbxCluster** ppCluster_;  // クラスタ情報 (関節事に関連つけられた頂点情報)
		int boneCount_;  // FBX に含まれている関節の数
		Bone* pBones_;  // 各関節の情報配列
		std::unordered_map<std::string, Bone*> boneNamePair_;  // 関節名とのペア
		Weight* pWeights_;  // ウェイト情報 (頂点に対する関節の影響度合い)
		Vertex* pVertexes_;  // 頂点情報
		DWORD** ppIndexData_;  // インデックス情報

		ID3D11Buffer** ppIndexBuffer_;
	};

}

