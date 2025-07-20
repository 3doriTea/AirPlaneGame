#pragma once
#include "cmtgb.h"
#include "Game.h"
#include "ComponentPool.h"

namespace mtgb
{
	/// <summary>
	/// ゲームオブジェクトにつけるコンポーネントのインタフェース
	/// </summary>
	/// <typeparam name="ComponentPoolT">コンポーネントプール</typeparam>
	/// <typeparam name="ComponentT">コンポーネント</typeparam>
	template<class ComponentPoolT, typename ComponentT>
	class IComponent
	{
		friend ComponentPoolT;
		friend ComponentPool<ComponentT>;
	public:
		IComponent() :
			entityId_{ -1 }
		{}
		IComponent(const EntityId _entityId);
		virtual ~IComponent();

		static ComponentT& Get(const EntityId _entityId);

		virtual void Initialize() {}

	private:
		EntityId entityId_;
	};

	template<class ComponentPoolT, typename ComponentT>
	inline IComponent<ComponentPoolT, ComponentT>::IComponent(const EntityId _entityId) :
		entityId_{ _entityId }
	{
		//Game::System<ComponentPoolT>().Register(_entityId);
	}

	template<class ComponentPoolT, typename ComponentT>
	inline IComponent<ComponentPoolT, ComponentT>::~IComponent()
	{
		//Game::System<ComponentPoolT>().UnRegister(entityId_);
	}

	template<class ComponentPoolT, typename ComponentT>
	inline ComponentT& IComponent<ComponentPoolT, ComponentT>::Get(const EntityId _entityId)
	{
		return Game::System<ComponentPoolT>().Get(_entityId);
	}
}
