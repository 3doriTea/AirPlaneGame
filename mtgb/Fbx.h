#pragma once
#include <vector>
#include <map>
#include <fbxsdk.h>
#include "Vector3.h"
#include "IModelPack.h"
#include "ISystem.h"
#include "FbxModel.h"
#include "Handlers.h"

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
		/// Fbxファイルを読み込みする
		/// </summary>
		/// <param name="_fileName">3Dモデルのファイル名</param>
		static FBXModelHandle Load(const std::string& _fileName);
		/// <summary>
		/// Fbxモデルを描画する
		/// </summary>
		/// <param name="_transfrom">座標系</param>
		/// <param name="_frame">アニメーションフレーム</param>
		void Draw(const FBXModelHandle _hModel, const Transform& _transfrom, int _frame);
		/// <summary>
		/// 解放処理
		/// </summary>
		void Release();

		

	private:
		FbxManager* pFbxManager_;  // Fbxファイルを扱う本体

		FBXModelHandle handleCounter_;              // ハンドラの加算用
		std::map<FBXModelHandle, FbxModel*> pFbxModels_;  // 読み込まれたスプライト
	};
}
