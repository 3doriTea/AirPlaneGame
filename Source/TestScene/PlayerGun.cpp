#include "PlayerGun.h"
#include "../TargetingSystem.h"

namespace
{
	const float GUNNER_COOL_TIME_SEC{ 0.1024f };
	const float PILOT_COOL_TIME_SEC{ 0.22f };
}

PlayerGun::PlayerGun(mtgb::WindowContext _wc, mtgb::GameObjectLayer _layer, mtgb::Transform* _transform, float _lockonside) 
	: cooldownTimer_(0.0f)
	, cooldownCount_(0.0f)
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = _lockonside;

	CircleDetectorConfig config =
	{
		.center = rectCenter,
		.radius = lockOnSide,
	};
	config.maxDistance = 50.0f;
	config.minDistance = 0.0f;
	config.targetTag = GameObjectTag::Enemy;
	config.windowContext = _wc;
	config.uiParams.layerFlag = _layer;

	pTargetingSystem_ = new TargetingSystem(_transform,config);
	pTargetingSystem_->uiParams.layerFlag = _layer;

	if (_wc == WindowContext::First)
	{
		cooldownCount_ = PILOT_COOL_TIME_SEC;
	}
	else if (_wc == WindowContext::Second)
	{
		cooldownCount_ = GUNNER_COOL_TIME_SEC;
	}
}

PlayerGun::~PlayerGun()
{
	delete pTargetingSystem_;
}

void PlayerGun::Update()
{
	if (cooldownTimer_ > 0.0f) {
		cooldownTimer_ -= Time::DeltaTimeF(); // 1ƒtƒŒ[ƒ€‚ÌŒo‰ß•b”‚É‡‚í‚¹‚ÄŒ¸­
	}
	pTargetingSystem_->SearchTargets();
}

void PlayerGun::Fire()
{
	if (cooldownTimer_ <= 0.0f)
	{
		pTargetingSystem_->FireAtTarget();
		cooldownTimer_ = cooldownCount_;
	}
}


void PlayerGun::Draw()
{
	pTargetingSystem_->DrawUI();
}