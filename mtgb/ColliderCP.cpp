#include "ColliderCP.h"
#include "RigidBody.h"

mtgb::ColliderCP::ColliderCP()
{
}

mtgb::ColliderCP::~ColliderCP()
{
}

void mtgb::ColliderCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].onColliders_.clear();
		}
	}

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY && i < poolId_.size() - 1)
		{
			for (size_t j = i + 1; j < poolId_.size(); j++)
			{
				if (poolId_[j] != INVALD_ENTITY)
				{
					if (pool_[i].IsHit(pool_[j]))
					{
						pool_[i].onColliders_.insert(&pool_[j]);
						pool_[j].onColliders_.insert(&pool_[i]);
					}
				}
			}
		}
	}
}
