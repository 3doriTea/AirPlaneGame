#pragma once

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

		virtual void Draw(Transform* _pTransform, int _frame) = 0;
		virtual void Release() = 0;
	};
}
