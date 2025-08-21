#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "RigidBody.h"

namespace mtgb
{
	class RigidBody;

	class RigidBodyCP : public ComponentPool<RigidBody>
	{
	public:
		RigidBodyCP();
		~RigidBodyCP();

		void Update() override;
	};
}
