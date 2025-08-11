#include "RigidBodyCP.h"
#include "Transform.h"
#include "GameTime.h"
#include "ColliderCP.h"

mtgb::RigidBodyCP::RigidBodyCP()
{
}

mtgb::RigidBodyCP::~RigidBodyCP()
{
}

void mtgb::RigidBodyCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			std::vector<Collider*> colliders{};
			Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);
			//pool_[i]
			for (auto& collider : colliders)
			{
				for (auto& hitCollider : collider->onColliders_)
				{
					pool_[i].onHit_(hitCollider->GetEntityId());
				}
			}

			pool_[i].pTransform_->position += pool_[i].velocity_ * Time::DeltaTimeF();
		}
	}
}
