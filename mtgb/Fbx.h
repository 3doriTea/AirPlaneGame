#pragma once
#include <vector>
#include <fbxsdk.h>
#include "Vector3.h"
#include "IModelPack.h"
#include "ISystem.h"

#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")

namespace mtgb
{
	class FbxParts;
	/// <summary>
	/// 3DモデルのFbxを読み込みするクラス
	/// </summary>
	class Fbx : public ISystem
	{
		friend class FbxParts;

	public:
		Fbx();
		~Fbx();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// FbxManagerを取得する
		/// </summary>
		/// <returns>FbxManagerのポインタ</returns>
		inline FbxManager* GetFbxManager() { return pFbxManager_; }
		/// <summary>
		/// FbxSceneを取得する
		/// </summary>
		/// <returns>FbxSceneのポインタ</returns>
		inline FbxScene* GetFbxScene() { return pFbxScene_; }

		/// <summary>
		/// Fbxファイルを読み込みする
		/// </summary>
		/// <param name="_fileName">3Dモデルのファイル名</param>
		static int Load(const std::string& _fileName);
		/// <summary>
		/// Fbxモデルを描画する
		/// </summary>
		/// <param name="_transfrom">座標系</param>
		/// <param name="_frame">アニメーションフレーム</param>
		void Draw(int _hModel, const Transform& _transfrom, int _frame);
		/// <summary>
		/// 解放処理
		/// </summary>
		void Release();

		/// <summary>
		/// 任意のボーンの位置を取得
		/// </summary>
		/// <param name="_boneName">ボーンの名前</param>
		Vector3 GetBonePosition(std::string _boneName);
		/// <summary>
		/// スキンメッシュアニメ中の現在の任意のボーンの位置を取得
		/// </summary>
		/// <param name="_boneName">ボーンの名前</param>
		Vector3 GetAnimBonePosition(std::string _boneName);

	private:
		std::vector<FbxParts*> pParts_;  // 複数あるかもしれないパーツ

		FbxManager* pFbxManager_;  // Fbxファイルを扱う本体
		FbxScene* pFbxScene_;  // Fbxファイル内のシーン

		FbxTime::EMode frameRate_;  // アニメーションフレームレート
		float animationSpeed_;  // アニメーションの再生速度
		int startFrame_;  // アニメーション最初のフレーム
		int endFrame_;  // アニメーション最後のフレーム

		/// <summary>
		/// ノードの中身を調べる
		/// </summary>
		/// <param name="_pNode">調べたいノード</param>
		/// <param name="_parts">パーツリスト</param>
		void CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _parts);
	};
}
