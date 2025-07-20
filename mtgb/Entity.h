#pragma once
#include "cmtgb.h"

namespace mtgb
{
	/// <summary>
	/// ECSのEntity
	/// </summary>
	class Entity
	{
	public:
		Entity() :
			entityId_{ ++entityCounter_ }  // id counterを加算してIdを入れる
		{
		}
		virtual ~Entity()
		{
		}

		/// <summary>
		/// エンティティIdを取得する
		/// </summary>
		/// <returns>エンティティ識別番号</returns>
		inline const EntityId GetEntityId() const { return entityId_; }

	protected:
		EntityId entityId_;  // エンティティの識別子

	private:
		static EntityId entityCounter_;  // エンティティ識別子のカウンタ
	};
}
