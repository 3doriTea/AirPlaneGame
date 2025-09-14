#include "TargetingSystem.h"
#include <algorithm>
#include "Bullet.h"
#include "DrawScreenUtility.h"

void TargetingSystem::Initialize(Transform* owner, const Vector2F& screenCenter, float detectionSize)
{
	// Transform設定
	ownerTransform = owner;
	
	// ターゲット検出設定
	targetDetector.config.detectionRect = 
	{
			screenCenter.x - detectionSize / 2.0f,
			screenCenter.y - detectionSize / 2.0f,
			detectionSize,
			detectionSize
	};
}

TargetingSystem::TargetingSystem()
	: ownerTransform{ nullptr }
	, reticleRadius{ 0.0f }
	, reticleRect{}
	, detectionFrameImage{ -1 }
	, targetReticleImage{ -1 }
{
	currentTarget = nullptr;

	// レティクルサイズを設定
	reticleRadius = 30.0f;

	// 画像を読み込む
	detectionFrameImage = Image::Load("Image/lockOnFrame.png");
	targetReticleImage = Image::Load("Image/lockOnReticle.png");

	// レティクル矩形のサイズを設定
	reticleRect.size = { reticleRadius * 2.0f, reticleRadius * 2.0f };


	// 検出距離、検出対象名設定
	targetDetector.config.maxDistance = 100.0f;
	targetDetector.config.targetName = "Enemy";

}

TargetingSystem::~TargetingSystem()
{
}

void TargetingSystem::SearchTargets()
{
	targetDetector.UpdateDetection();

	// ワールド座標系で一番近い敵を狙う
	auto it = std::min_element(
		targetDetector.detectedTargets.begin(),
		targetDetector.detectedTargets.end(),
		[this](const RectContainsInfo& a, const RectContainsInfo& b)
		{
			float distanceA = (ownerTransform->position - a.worldPos).Size();
			float distanceB = (ownerTransform->position - b.worldPos).Size();
			return distanceA < distanceB;
		}
	);

	if (it != targetDetector.detectedTargets.end())
	{
		currentTarget = &(*it); // 最も近い敵をターゲットに設定

		Vector2F ratio = Game::System<Screen>().GetSizeRatio();
		
		reticleRect.x =  currentTarget->screenPos.x - reticleRadius * ratio.x;
		reticleRect.y = currentTarget->screenPos.y - reticleRadius * ratio.y;
		reticleRect.width = (reticleRadius * 2.0f);
		reticleRect.height = (reticleRadius * 2.0f);
		//reticleRect.x = (currentTarget->screenPos.x - reticleRadius) / ratio.x;
		//reticleRect.y = (currentTarget->screenPos.y - reticleRadius) / ratio.y;
		//reticleRect.width = (reticleRadius * 2.0f) / ratio.x;
		//reticleRect.width = reticleRadius * 2.0f * ratio.x;

	}
	else
	{
		currentTarget = nullptr; // ターゲットが見つからない場合
	}
}

void TargetingSystem::FireAtTarget()
{
	RigidBody& rb{ RigidBody::Get(ownerTransform->GetEntityId()) };
	Vector3 targetPosition{ Mathf::TargetingPosition(ownerTransform->GetWorldPosition(), currentTarget->worldPos, rb.velocity_, Bullet::GetMoveSpeed()) };
	Vector3 targetDirection;
	Quaternion fireDirection;

	if (HasTarget())
	{
		targetDirection = Vector3::Normalize(targetPosition - ownerTransform->GetWorldPosition());
	}
	// ターゲットがいない場合は正面方向に射撃
	else
	{
		targetDirection = ownerTransform->Forward();
	}
	fireDirection = Quaternion::LookRotation(targetDirection, Vector3::Up());
	GameObject::Instantiate<Bullet>(ownerTransform->GetWorldPosition(), fireDirection, Bullet::Shooter::Player);
}

mtgb::Vector3 TargetingSystem::GetCurrentTargetPosition() const
{
	if (HasTarget())
	{
		return currentTarget->worldPos;
	}
	return Vector3::Zero();
}

bool TargetingSystem::HasTarget() const
{
	return currentTarget != nullptr && targetDetector.HasDetectedTargets();
}

void TargetingSystem::DrawUI() const
{
	// ターゲット検出範囲を描画
	Vector2F ratio =  Game::System<Screen>().GetSizeRatio();
	//RectF detectionRect{ targetDetector.config.detectionRect.point / ratio,targetDetector.config.detectionRect.size / ratio };
	Draw::Image(detectionFrameImage, { targetDetector.config.detectionRect.point * ratio,targetDetector.config.detectionRect.size * ratio}, uiParams);

	// ターゲットがロックオンされている場合、レティクルを描画
	if (HasTarget())
	{
		Draw::Image(targetReticleImage, { reticleRect.point,reticleRect.size * ratio }, uiParams);
	}
}
