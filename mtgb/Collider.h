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
			TYPE_SPHERE,  // ��(���S����̈�苗��)
			TYPE_CAPSULE,  // �J�v�Z��(��������̈�苗��)
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
		Transform* pTransform_;  // TODO: ��Ȃ�Transform

		ColliderType type_;  // �����蔻��̌`

		union
		{
			struct
			{
				Vector3 offset_;  // �I�t�Z�b�g
				float radius_;  // ���a
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
