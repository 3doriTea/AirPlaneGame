#pragma once
#include <vector>
#include <fbxsdk.h>
#include "IModelPack.h"

#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")


namespace mtgb
{
	/// <summary>
	/// 3DモデルのFbxを読み込みするクラス
	/// </summary>
	class Fbx : public IModelPack
	{
		friend class FbxParts;

	public:
		Fbx();
		~Fbx();

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
		void Load(const std::string& _fileName) override;
		/// <summary>
		/// Fbxモデルを描画する
		/// </summary>
		/// <param name="_pTransform">座標系</param>
		/// <param name="_frame">アニメーションフレーム</param>
		void Draw(Transform* _pTransform, int _frame) override;
		/// <summary>
		/// 解放処理
		/// </summary>
		void Release() override;

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
