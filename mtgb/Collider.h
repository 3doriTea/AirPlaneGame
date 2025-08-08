#pragma once
#include "IComponent.h"
#include "ColliderCP.h"
#include <set>
#include <DirectXCollision.h>
#include "Vector3.h"

namespace mtgb
{
	class ColliderCP;
	class Transform;

	class Collider : public IComponent<ColliderCP, Collider>
	{
		friend ColliderCP;

	public:
		enum ColliderType : uint8_t
		{
			TYPE_SPHERE,  // 球(中心からの一定距離)
			TYPE_CAPSULE,  // カプセル(線分からの一定距離)
		};

	public:
		using IComponent<ColliderCP, Collider>::IComponent;
		Collider(const EntityId _entityId);
		~Collider();
		inline Collider& operator= (const Collider& _other)
		{
			if (&_other == this)
			{
				return *this;
			}



			return *this;
		}

		bool IsHit(const Collider& _other) const;

	private:
		Transform* pTransform_;  // TODO: 危ないTransform

		ColliderType type_;  // 当たり判定の形

		union
		{
			struct
			{
				Vector3 offset_;  // オフセット
				float radius_;  // 半径
			} sphere_;
			struct
			{

			} capsule_;
		};

		union
		{
			DirectX::BoundingSphere computeSphere_;
		};

		std::set<Collider*> onColliders_;
	};
}
