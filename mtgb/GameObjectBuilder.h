#pragma once
#include <string>
#include "Vector3.h"
#include "Quaternion.h"

namespace mtgb
{
	/// <summary>
	/// <para>ゲームオブジェクトを生成するときの設定</para>
	/// <para>そのまま構造体を使用可能だが、GameObjectBuilderを通すことを推奨</para>
	/// </summary>
	struct GAME_OBJECT_DESC
	{
		std::string name{};
		Vector3 position{ Vector3::Zero() };
		Vector3 scale{ Vector3::One() };
		Quaternion rotate{ Quaternion::Identity() };
		bool isActive{ true };
		bool callUpdate{ true };
		bool callDraw{ true };
		// padding 13byte
	};

	// MEMO: 構造体のそのまま初期化では、順番が固定されているが、
	//     : Builderを使うことで好きなタイミングで任意の値をセットできる

	/// <summary>
	/// <para>GAME_OBJECT_DESCのビルダークラス</para>
	/// <para>メンバのセッタ関数を立て続けに呼び出し、最終的にBuild関数でGAME_OBJECT_DESCを構築</para>
	/// </summary>
	class GameObjectBuilder
	{
	public:
		/// <summary>
		/// <para>以下のように使う</para>
		/// <para>GameObjectBuilder().SetName("Name").Set...().Build()</para>
		/// </summary>
		GameObjectBuilder()
		{}
		~GameObjectBuilder()
		{}

		GameObjectBuilder& SetName(const std::string& _name)     { desc_.name = _name;             return *this; }
		GameObjectBuilder& SetPosition(const Vector3& _position) { desc_.position = _position;     return *this; }
		GameObjectBuilder& SetRotate(const Quaternion& _rotate)  { desc_.rotate = _rotate;         return *this; }
		GameObjectBuilder& SetScale(const Vector3& _scale)       { desc_.scale = _scale;           return *this; }
		GameObjectBuilder& SetIsActive(const bool _isActive)     { desc_.isActive = _isActive;     return *this; }
		GameObjectBuilder& SetCallUpdate(const bool _callUpdate) { desc_.callUpdate = _callUpdate; return *this; }
		GameObjectBuilder& SetCallDraw(const bool _callDraw)     { desc_.callDraw = _callDraw;     return *this; }

		/// <summary>
		/// 設定をビルドする
		/// </summary>
		/// <returns>完成したGAME_OBJECT_DESCのコンスト参照ポインタ</returns>
		const GAME_OBJECT_DESC& Build() const { return desc_; }
	private:
		GAME_OBJECT_DESC desc_;
	};
}
