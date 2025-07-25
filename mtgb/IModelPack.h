#pragma once
#include <string>


namespace mtgb
{
	class Transform;

	/// <summary>
	/// 3Dモデル読み込み用抽象インタフェース
	/// </summary>
	class IModelPack
	{
	public:
		IModelPack() {}
		virtual ~IModelPack() {}

		/// <summary>
		/// 3Dモデルの読み込みをする
		/// </summary>
		/// <param name="_fileName">3Dモデルのファイル名</param>
		virtual void Load(const std::string& _fileName) = 0;
		/// <summary>
		/// 3Dモデルを描画する
		/// </summary>
		/// <param name="_transfrom">座標系</param>
		/// <param name="_frame">アニメーションフレーム</param>
		virtual void Draw(const Transform& _transfrom, int _frame) = 0;
		/// <summary>
		/// 解放処理
		/// </summary>
		virtual void Release() = 0;
	};
}
