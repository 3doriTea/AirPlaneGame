#include "TargetingSystem.h"
#include <algorithm>
#include "Bullet.h"
#include "DrawScreenUtility.h"
#include "CameraSystem.h"

TargetingSystem::TargetingSystem()
	: ownerTransform{ nullptr }
	, reticleRadius{ 0.0f }
	, reticleRect{}
	, detectionFrameImage{ -1 }
	, targetReticleImage{ -1 }
	, detectionType{ DetectionType::Rectangle }
	, detector{ nullptr }
{
	currentTarget = nullptr;

	// レティクルサイズを設定
	reticleRadius = 30.0f;

	// 画像を読み込む
	detectionFrameImage = Image::Load("Image/lockOnFrame.png");
	targetReticleImage = Image::Load("Image/lockOnReticle.png");

	// レティクル矩形のサイズを設定
	reticleRect.size = { reticleRadius * 2.0f, reticleRadius * 2.0f };

	// デフォルトは矩形検出器
	SetDetectionType(DetectionType::Rectangle);
}

TargetingSystem::~TargetingSystem()
{
}

void TargetingSystem::SetDetectionType(DetectionType type)
{
	/*detectionType = type;
	
	switch (type)
	{
	case DetectionType::Rectangle:
		detector = std::make_unique<RectDetector>(rectDetector);
		break;
	case DetectionType::Circle:
		detector = std::make_unique<CircleDetector>(circleDetector);
		break;
	case DetectionType::Ray:
		detector = std::make_unique<RayDetector>(rayDetector);
		break;
	}*/
}

void TargetingSystem::InitializeAsRect(Transform* owner, const Vector2F& screenCenter, float detectionSize)
{
	ownerTransform = owner;
	
	RectDetectorConfig config;
	config.detectionRect = 
	{
		screenCenter.x - detectionSize / 2.0f,
		screenCenter.y - detectionSize / 2.0f,
		detectionSize,
		detectionSize
	};
	config.maxDistance = 100.0f;
	config.targetTag = GameObjectTag::Enemy;
	
	rectDetector = RectDetector(config);
	SetDetectionType(DetectionType::Rectangle);
}

void TargetingSystem::InitializeAsCircle(Transform* owner, const Vector2F& screenCenter, float radius)
{
	ownerTransform = owner;
	
	CircleDetectorConfig config;
	config.center = screenCenter;
	config.radius = radius;
	config.maxDistance = 100.0f;
	config.targetTag = GameObjectTag::Enemy;
	
	circleDetector = CircleDetector(config);
	SetDetectionType(DetectionType::Circle);
}

void TargetingSystem::InitializeAsRay(Transform* owner, const Vector3& rayDirection, float maxAngleDegrees, float maxDistance)
{
	ownerTransform = owner;
	
	RayDetectorConfig config;
	config.rayOrigin = owner->GetWorldPosition();
	config.rayDirection = rayDirection;
	config.maxAngleDegrees = maxAngleDegrees;
	config.maxDistance = maxDistance;
	config.targetTag = GameObjectTag::Enemy;
	
	rayDetector = RayDetector(config);
	SetDetectionType(DetectionType::Ray);
}

void TargetingSystem::Initialize(Transform* owner, const Vector2F& screenCenter, float detectionSize)
{
	// 下位互換性のため矩形検出器として初期化
	InitializeAsRect(owner, screenCenter, detectionSize);
}



void TargetingSystem::SearchTargets()
{
	if (!detector)
	{
		return;
	}

	// レイ検出の場合は起点を更新
	if (detectionType == DetectionType::Ray && ownerTransform)
	{
		rayDetector.config.rayOrigin = ownerTransform->GetWorldPosition();
		rayDetector.config.rayDirection = ownerTransform->Forward();
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
		if ((*it).screenPos.z > 0.0f && (*it).screenPos.z < 1.0f)
		{
			currentTarget = const_cast<ScreenCoordContainsInfo*>(&(*it)); // 最も近い敵をターゲットに設定
		}
	}
	else
	{
		currentTarget = nullptr; // ターゲットが見つからない場合
	}
}

void TargetingSystem::FireAtTarget()
{
	Vector3 targetDirection;
	Quaternion fireDirection;

	if (HasTarget())
	{
		RigidBody& rb{ RigidBody::Get(currentTarget->entityId) };
		Vector3 targetPosition{ Mathf::TargetingPosition(ownerTransform->GetWorldPosition(), currentTarget->worldPos, -rb.velocity_, Bullet::GetMoveSpeed()) };
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
	return currentTarget != nullptr && detector && detector->HasDetectedTargets();
}

void TargetingSystem::DrawUI() const
{
	// 検出範囲の描画（検出方式に応じて）
	Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	float scale = (std::min)(ratio.x, ratio.y);

	switch (detectionType)
	{
	case DetectionType::Rectangle:
	{
		float scaledSize = rectDetector.config.detectionRect.size.x * scale;
		Vector2F center = Game::System<Screen>().GetSizeF() * 0.5f;
		Vector2F newPoint = center - Vector2F{scaledSize, scaledSize} * 0.5f;
		RectF drawRect = { newPoint,{scaledSize,scaledSize} };
		Draw::Image(detectionFrameImage, drawRect, uiParams);
		break;
	}
	case DetectionType::Circle:
	{
		float scaledRadius = circleDetector.config.radius * scale;
		Vector2F center = circleDetector.config.center * scale;
		RectF drawRect = { 
			center.x - scaledRadius, center.y - scaledRadius, 
			scaledRadius * 2.0f, scaledRadius * 2.0f 
		};
		Draw::Image(detectionFrameImage, drawRect, uiParams);
		break;
	}
	case DetectionType::Ray:
		// レイの場合は方向指示などを描画（実装は省略）
		break;
	}

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
