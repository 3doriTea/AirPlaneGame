#include "RigidBodyCP.h"
#include "Transform.h"
#include "GameTime.h"

mtgb::RigidBodyCP::RigidBodyCP()
{
}

mtgb::RigidBodyCP::~RigidBodyCP()
{
}

void mtgb::RigidBodyCP::Initialize()
{
}

void mtgb::RigidBodyCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].pTransform_->position_ += pool_[i].velocity_ * Time::DeltaTimeF();
		}
	}
}
