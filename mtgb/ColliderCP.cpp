#include "ColliderCP.h"
#include "RigidBody.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Fbx.h"
#include "Debug.h"

#include "GameObject.h"
#include "RectInt.h"
#include "SceneSystem.h"
#include "CameraSystem.h"

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
		&& "SphereColliderÉÇÉfÉãÇÃì«Ç›çûÇ›Ç…é∏îs @ColliderCP::Start");
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
				// TODO: ÉJÉvÉZÉãå^ÇÃåvéZ
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
						LOGIMGUI("Add %d and %d", pool_[i].GetEntityId(), pool_[j].GetEntityId());
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
			LOGIMGUI("EID:%d colSize=%d ", poolId_[i], pool_[i].onColliders_.size());
		}
	}
	LOGF("ColliderCP END\n");
}
mtgb::EntityId mtgb::ColliderCP::RaycastHit(const Vector3& _origin, const Vector3& _dir, float dist)
{
	EntityId nearestEntity = INVALD_ENTITY;

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			std::vector<Collider*> colliders{};
			Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);

			float nearest = dist;
			Transform* pTransform;
			float distance = 0.0f;
			//pool_[i]
			for (auto& collider : colliders)
			{
				if (collider->IsHit(_origin, _dir, &distance))
				{
					Game::System<TransformCP>().TryGet(pTransform, collider->GetEntityId());
					if(distance < nearest)
					{
						nearest = distance;
						nearestEntity = collider->GetEntityId();
					}
				}
			}
		}
	}
	return nearestEntity;
}

void mtgb::ColliderCP::RectContains(const RectInt& _rect, const std::string& _name, std::vector<GameObject*>* _pRectContainsGameObject, WindowContext _context)
{
	_pRectContainsGameObject->clear();

	std::vector<GameObject*> pFoundGameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, &pFoundGameObjects);
	if (pFoundGameObjects.empty()) return;

	CameraSystem& camSys = Game::System<CameraSystem>();
	const WorldToScreenData& data = camSys.GetWorldToScreenData(_context);

	for (auto& object : pFoundGameObjects)
	{
		Vector2Int screenPos = camSys.WorldToScreen(object->Component<Transform>()->position, data);
		if (screenPos.x < 0 || screenPos.y < 0)
			continue;
		if (RectInt::Contains(screenPos, _rect))
		{
			_pRectContainsGameObject->push_back(object);
		}
	}
}
