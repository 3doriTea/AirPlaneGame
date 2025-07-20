#pragma once
#include "IComponent.h"
#include "RigidBodyCP.h"
#include "Vector3.h"

namespace mtgb
{
	class RigidBodyCP;
	class Transform;

	class RigidBody : public IComponent<RigidBodyCP, RigidBody>
	{
		friend RigidBodyCP;
	public:
		using IComponent<RigidBodyCP, RigidBody>::IComponent;
		RigidBody(const EntityId _entityId);
		~RigidBody();
		inline RigidBody& operator=(const RigidBody& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->velocity_ = _other.velocity_;

			return *this;
		}

	public:
		Vector3 velocity_;  // ‘¬“x

	private:
		Transform* pTransform_;  // TODO: Šë‚È‚¢Transform
	};
}
