#include "PlayerGun.h"
#include "../TargetingSystem.h"

namespace
{
	const float COOL_TIME_SEC{ 1.0f };
}

PlayerGun::PlayerGun(mtgb::WindowContext _wc, mtgb::GameObjectLayer _layer, mtgb::Transform* _transform, float _lockonside) : coolTime_(0.0f)
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = _lockonside;

	pTargetingSystem_ = new TargetingSystem();
	pTargetingSystem_->Initialize(_transform, rectCenter, lockOnSide);
	pTargetingSystem_->targetDetector.config.windowContext = _wc;
	pTargetingSystem_->uiParams.layerFlag = _layer;
}

PlayerGun::~PlayerGun()
{
	delete pTargetingSystem_;
}

void PlayerGun::Update()
{
	if (coolTime_ > 0.0f) {
		coolTime_ -= Time::DeltaTimeF(); // 1ƒtƒŒ[ƒ€‚ÌŒo‰ß•b”‚É‡‚í‚¹‚ÄŒ¸­
	}
	pTargetingSystem_->SearchTargets();
}

void PlayerGun::Fire()
{
	if (coolTime_ <= 0.0f)
	{
		pTargetingSystem_->FireAtTarget();
		coolTime_ = COOL_TIME_SEC;
	}
}


void PlayerGun::Draw()
{
	pTargetingSystem_->DrawUI();
}