#include "TargetingSystem.h"
#include <algorithm>
#include "Bullet.h"
#include "DrawScreenUtility.h"
#include "CameraSystem.h"

namespace
{
	const Vector3 FIRE_POS_OFFSET{ 0,-0.5,0};
}



TargetingSystem::~TargetingSystem()
{
	Game::System<EventManager>().GetEvent<ThreatEventData>().Unsubscribe(threatEventHandlerId_);
	SAFE_DELETE(detector);
}

ImageHandle TargetingSystem::GetDetectionFrameImage() const
{
	switch (currentThreatLevel_)
	{
	case ThreatLevel::Normal:
		return normalDetectionFrameImage_;
	case ThreatLevel::Danger:
		return alertDetectionFrameImage_;
	default:
		return normalDetectionFrameImage_;
	}
}

void TargetingSystem::SearchTargets()
{
	if (!detector)
	{
		return;
	}

	detector->UpdateDetection();

	const auto& detectedTargets = detector->GetDetectedTargets();

	// ワールド座標系で一番近い敵を狙う
	auto it = std::min_element(
		detectedTargets.begin(),
		detectedTargets.end(),
		[this](const ScreenCoordContainsInfo& a, const ScreenCoordContainsInfo& b)
		{
			float distanceA = (ownerTransform->GetWorldPosition() - a.worldPos).Size();
			float distanceB = (ownerTransform->GetWorldPosition() - b.worldPos).Size();
			return distanceA < distanceB;
		}
	);

	if (it != detectedTargets.end())
	{
		currentTarget = const_cast<ScreenCoordContainsInfo*>(&(*it)); // 最も近い敵をターゲットに設定
	}
	else
	{
		currentTarget = nullptr; // ターゲットが見つからない場合
	}
}

void TargetingSystem::ClearTarget()
{
	currentTarget = nullptr;
}

void TargetingSystem::FireAtTarget()
{
	Vector3 targetDirection;
	Vector3 projectilePos = ownerTransform->GetWorldPosition()
		+ ownerTransform->Right() * FIRE_POS_OFFSET.x
		+ ownerTransform->Up() * FIRE_POS_OFFSET.y
		+ ownerTransform->Forward() * FIRE_POS_OFFSET.z;
	Quaternion fireDirection;

	if (HasTarget())
	{
		RigidBody& rb{ RigidBody::Get(currentTarget->entityId) };
		Vector3 targetPosition{ Mathf::TargetingPosition(projectilePos, currentTarget->worldPos, -rb.velocity_, Bullet::GetMoveSpeed()) };
		targetDirection = Vector3::Normalize(targetPosition - projectilePos);
	}
	// ターゲットがいない場合は正面方向に射撃
	else
	{
		targetDirection = ownerTransform->Forward();
	}
	fireDirection = Quaternion::LookRotation(targetDirection,Vector3::Up() );
	GameObject::Instantiate<Bullet>(projectilePos, fireDirection, Bullet::Shooter::Player);
	//GameObject::Instantiate<Bullet>(projectilePos, fireDirection, Bullet::Shooter::Player);
}

mtgb::Vector3 TargetingSystem::GetCurrentTargetPosition() const
{
	if (HasTarget())
	{
		return currentTarget->worldPos;
	}
	return Vector3::Zero();
}

void TargetingSystem::OnThreatLevelChanged(const ThreatEventData& _data)
{
	currentThreatLevel_ = _data.level;
}

bool TargetingSystem::HasTarget() const
{
	return currentTarget != nullptr && detector && detector->HasDetectedTargets();
}

void TargetingSystem::DrawUI() const
{
	// ターゲット検出範囲を描画
	Draw::Image(GetDetectionFrameImage(), detector->GetDetectionArea(), uiParams);

	// ターゲットがロックオンされている場合、レティクルを描画
	if (HasTarget())
	{
		Vector2F reticlePos = { currentTarget->screenPos.x, currentTarget->screenPos.y };
		RectF reticleDrawRect = { 
			reticlePos.x - reticleRadius, reticlePos.y - reticleRadius,
			reticleRadius * 2.0f, reticleRadius * 2.0f 
		};
		Draw::Image(targetReticleImage, reticleDrawRect, uiParams);
	}
}
