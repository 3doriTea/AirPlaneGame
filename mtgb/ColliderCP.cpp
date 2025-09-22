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
#include "Screen.h"
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

	Collider::hBoxModel_ = Fbx::Load("Model/BoxCollider.fbx");
	massert(Collider::hBoxModel_ >= 0
		&& "BoxColliderÉÇÉfÉãÇÃì«Ç›çûÇ›Ç…é∏îs @ColliderCP::Start");
}

void mtgb::ColliderCP::Update()
{
	static Matrix4x4 matrix{};
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].onColliders_.clear();

			pool_[i].UpdateBoundingData();

			//switch (pool_[i].type_)
			//{
			//case Collider::TYPE_CAPSULE:
			//	// TODO: ÉJÉvÉZÉãå^ÇÃåvéZ
			//	break;
			//case Collider::TYPE_SPHERE:
			//	pool_[i].pTransform_->GenerateWorldMatrix(&matrix);
			//	pool_[i].computeSphere_.Center = pool_[i].sphere_.offset_ * matrix;
			//	pool_[i].computeSphere_.Radius = pool_[i].sphere_.radius_;
			//	break;
			//default:
			//	break;
			//}
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
						
						/*LOGF("Add %d and %d\n", pool_[i].GetEntityId(), pool_[j].GetEntityId());
						LOGIMGUI("Add %d and %d", pool_[i].GetEntityId(), pool_[j].GetEntityId());*/
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
mtgb::EntityId mtgb::ColliderCP::RayCastHitAll(const Vector3& _origin, const Vector3& _dir, float dist)
{
	EntityId nearestEntity = INVALD_ENTITY;

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			/*std::vector<Collider*> colliders{};
			Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);*/

			float nearest = dist;
			Transform* pTransform{ nullptr };
			float distance = 0.0f;
			//pool_[i]
			//for (auto& collider : colliders)
			{
				//if (collider->IsHit(_origin, _dir, &distance))
				EntityId id = poolId_[i];
				if(RayCastHit(_origin,_dir,&distance,id))
				{
					//Game::System<TransformCP>().TryGet(pTransform, id);
					if(distance < nearest)
					{
						nearest = distance;
						nearestEntity = id;
					}
				}
			}
		}
	}
	return nearestEntity;
}

bool mtgb::ColliderCP::RayCastHit(const Vector3& _origin, const Vector3& _dir, float* dist, EntityId _entityId)
{
	std::vector<Collider*> colliders{};
	if (!TryGet(&colliders, _entityId))
		return false;

	for (auto& collider : colliders)
	{
		return collider->IsHit(_origin, _dir, dist);
	}
	return false;
}

void mtgb::ColliderCP::IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds)
{
	_entityIds->clear();

	for (const auto& collider : pool_)
	{
		if (collider.IsHit(_center, _radius))
		{
			_entityIds->push_back(collider.GetEntityId());
		}
	}
}

void mtgb::ColliderCP::RectContains(const RectF& _rect, const std::string& _name, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	_info->clear();

	std::vector<GameObject*> foundGameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, &foundGameObjects);
	if (foundGameObjects.empty()) return;
	RectContainsImpl(_rect, foundGameObjects, _info, _context);
}

void mtgb::ColliderCP::RectContains(const RectF& _rect, GameObjectTag _tag, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	_info->clear();

	std::vector<GameObject*> foundGameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_tag, &foundGameObjects);
	if (foundGameObjects.empty()) return;
	RectContainsImpl(_rect, foundGameObjects, _info, _context);
}

void mtgb::ColliderCP::RectContainsImpl(const RectF& _rect,const std::vector<GameObject*>& _objs, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	CameraSystem& camSys = Game::System<CameraSystem>();
	const WorldToScreenData& data = camSys.GetWorldToScreenData(_context);
	Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	for (auto& object : _objs)
	{
		Vector3 worldPos = object->Component<Transform>()->GetWorldPosition();
		Vector3 screenPos = camSys.GetWorldToScreenPos(worldPos, data);
		/*if (screenPos.z < 0)
			continue;*/
			/*if (RectF::Contains(screenPos, RectF(_rect.x ,_rect.y , _rect.width , _rect.height )))
			{
				_info->emplace_back(worldPos,screenPos,object->GetEntityId());
			}*/
		if (RectF::Contains(Vector2F{ screenPos.x,screenPos.y }, RectF(_rect.x * ratio.x, _rect.y * ratio.y, _rect.width * ratio.x, _rect.height * ratio.y)))
		{
			_info->emplace_back(worldPos, screenPos, object->GetEntityId());
		}
	}
}
