#pragma once
#include "cmtgb.h"

namespace mtgb
{
	using EntityId = int64_t;

	/// <summary>
	/// �R���|�[�l���g�v�[���̃C���^�t�F�[�X
	/// </summary>
	class IComponentPool
	{
	public:
		IComponentPool();
		virtual ~IComponentPool();

		virtual void Remove(const EntityId _entityId) = 0;
	};
}
