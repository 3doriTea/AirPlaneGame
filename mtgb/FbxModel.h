#pragma once
#include <vector>
#include "Vector3.h"
#include <fbxsdk.h>
#include <string>

#include "IModelPack.h"


namespace mtgb
{
	class FbxParts;

	/// <summary>
	/// 1つのFbxファイルを扱うクラス
	/// </summary>
	class FbxModel : public IModelPack
	{
		friend class FbxParts;

	public:
		FbxModel();
		~FbxModel();

		void Load(const std::string& _fileName) override;

		void Draw(const Transform& _transfrom, int _frame) override;

		void Release() override;

		/// <summary>
		/// FbxSceneを取得する
		/// </summary>
		/// <returns>FbxSceneのポインタ</returns>
		inline FbxScene* GetFbxScene() { return pFbxScene_; }

		/// <summary>
		/// Fbxパーツを取得する
		/// </summary>
		/// <param name="_index">インデックス</param>
		/// <returns>Fbxパーツのポインタ</returns>
		inline FbxParts* GetFbxParts(const int _index) { return pParts_[_index]; }

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
		
		/// <summary>
		/// ロードしたときのモデルファイルの名前を取得
		/// </summary>
		/// <returns>ファイル名</returns>
		std::string GetFileName() const { return fileName_; }
	private:

		/// <summary>
		/// ノードの中身を調べる
		/// </summary>
		/// <param name="_pNode">調べたいノード</param>
		/// <param name="_parts">パーツリスト</param>
		void CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _parts);


	private:
		std::vector<FbxParts*> pParts_;  // 複数あるかもしれないパーツ

		FbxScene* pFbxScene_;  // Fbxファイル内のシーン

		FbxTime::EMode frameRate_;  // アニメーションフレームレート
		float animationSpeed_;  // アニメーションの再生速度
		int startFrame_;  // アニメーション最初のフレーム
		int endFrame_;  // アニメーション最後のフレーム

		std::string fileName_;  // モデルファイルのパス
	};
}
