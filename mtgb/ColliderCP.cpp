#include "ColliderCP.h"
#include "RigidBody.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Fbx.h"
#include "Debug.h"

mtgb::ColliderCP::ColliderCP()
{
}

mtgb::ColliderCP::~ColliderCP()
{
}

void mtgb::ColliderCP::Start()
{
	Collider::hSphereModel_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(Collider::hSphereModel_ >= 0
		&& "SphereColliderƒ‚ƒfƒ‹‚Ì“Ç‚İ‚İ‚É¸”s @ColliderCP::Start");
}

void mtgb::ColliderCP::Update()
{
	static Matrix4x4 matrix{};
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].onColliders_.clear();

			switch (pool_[i].type_)
			{
			case Collider::TYPE_CAPSULE:
				// TODO: ƒJƒvƒZƒ‹Œ^‚ÌŒvZ
				break;
			case Collider::TYPE_SPHERE:
				pool_[i].pTransform_->GenerateWorldMatrix(&matrix);
				pool_[i].computeSphere_.Center = pool_[i].sphere_.offset_ * matrix;
				pool_[i].computeSphere_.Radius = pool_[i].sphere_.radius_;
				break;
			default:
				break;
			}
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
						
						LOGF("Add %d and %d\n", pool_[i].GetEntityId(), pool_[j].GetEntityId());
					}
				}
			}
		}
	}
}

void mtgb::ColliderCP::TestDraw() const
{

	//Game::System<MTImGui>().Begin("ColliderCP");
	LOGF("ColliderCP BEGIN\n");
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			LOGF("EID:%d colSize=%d\n ", poolId_[i], pool_[i].onColliders_.size());
		}
	}
	LOGF("ColliderCP END\n");
}
