#pragma once
#include "cmtgb.h"
#include <functional>

namespace mtgb
{
	using EntityId = int64_t;

	/// <summary>
	/// コンポーネントプールのインタフェース
	/// </summary>
	class IComponentPool
	{
	public:
		IComponentPool();
		virtual ~IComponentPool();

		virtual void Remove(const EntityId _entityId) = 0;

	protected:
		void RegisterCurrentScene(const std::function<void()>& _onMove);
	};
}
