#pragma once

namespace mtgb
{
	class Transform;

	/// <summary>
	/// 3D���f���ǂݍ��ݗp���ۃC���^�t�F�[�X
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
