#include "PlayerGun.h"
#include "../TargetingSystem.h"

namespace
{
	const float GUNNER_COOL_TIME_SEC{ 0.15f };
	const float PILOT_COOL_TIME_SEC{ 0.22f };
}

PlayerGun::PlayerGun(mtgb::WindowContext _wc, mtgb::GameObjectLayer _layer, mtgb::Transform* _transform, mtgb::RigidBody* _rigidBody, float _lockonside)
	: cooldownTimer_(0.0f)
	, cooldownCount_(0.0f)
	, targetingSystem_{ _transform, _rigidBody,
	CircleDetectorConfig
		{
	.base = 
		{
			.targetTag = GameObjectTag::Enemy,
			.windowContext = _wc,
			.maxDistance = 300.0f,
			.minDistance = 0.0f,
			.uiParams =
			{
			.layerFlag = _layer
			},
		},
	.center = Game::System<Screen>().GetSize() / 2.0f,
	.radius = _lockonside
		}
	}
{
	if (_wc == WindowContext::First)
	{
		cooldownCount_ = PILOT_COOL_TIME_SEC;
	}
	else if (_wc == WindowContext::Second)
	{
		cooldownCount_ = GUNNER_COOL_TIME_SEC;
	}
}
PlayerGun::PlayerGun(mtgb::WindowContext _wc, mtgb::GameObjectLayer _layer, mtgb::Transform* _transform, float _lockonside)
	: PlayerGun{_wc,_layer,_transform,&RigidBody::Get(_transform->GetEntityId()),_lockonside}
{
}

PlayerGun::~PlayerGun()
{
}

void PlayerGun::Update()
{
	if (cooldownTimer_ > 0.0f) {
		cooldownTimer_ -= Time::DeltaTimeF(); // 1ÉtÉåÅ[ÉÄÇÃåoâﬂïbêîÇ…çáÇÌÇπÇƒå∏è≠
	}
	targetingSystem_.SearchTargets();
}

void PlayerGun::Fire()
{
	if (cooldownTimer_ <= 0.0f)
	{
		targetingSystem_.FireAtTarget();
		cooldownTimer_ = cooldownCount_;
	}
}


void PlayerGun::Draw()
{
	targetingSystem_.DrawUI();
}