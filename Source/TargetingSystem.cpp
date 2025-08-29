#include "TargetingSystem.h"
#include <algorithm>
#include "TestScene/PlayerBullet.h"

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
	delete currentTarget;
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
		reticleRect.x = currentTarget->screenPos.x - reticleRadius;
		reticleRect.y = currentTarget->screenPos.y - reticleRadius;
	}
	else
	{
		currentTarget = nullptr; // ターゲットが見つからない場合
	}
}

void TargetingSystem::FireAtTarget()
{
	if (HasTarget())
	{
		Vector3 targetDirection = Vector3::Normalize(currentTarget->worldPos - ownerTransform->GetWorldPosition());
		Quaternion fireDirection = Quaternion::LookRotation(targetDirection, Vector3::Up());
		GameObject::Instantiate<PlayerBullet>(ownerTransform->GetWorldPosition(), fireDirection);
	}
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
	Draw::Image(detectionFrameImage, targetDetector.config.detectionRect, uiParams);

	// ターゲットがロックオンされている場合、レティクルを描画
	if (HasTarget())
	{
		Draw::Image(targetReticleImage, reticleRect, uiParams);
	}
}
