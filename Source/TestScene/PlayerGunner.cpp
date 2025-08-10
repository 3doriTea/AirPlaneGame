#include "PlayerGunner.h"
#include "PlayerBullet.h"

using namespace mtgb;

PlayerGunner::PlayerGunner(const EntityId _plane) : GameObject(GameObjectBuilder()
	.Build()),
	pTransform_{ Component<Transform>() }
{
	pTransform_->SetParent(_plane);
}

PlayerGunner::~PlayerGunner()
{
}

void PlayerGunner::Update()
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };

	if (InputUtil::GetKey(KeyCode::W))
	{
		pTransform_->Rotation(Vector3::Left() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		pTransform_->Rotation(Vector3::Right() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		pTransform_->Rotation(Vector3::Down() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		pTransform_->Rotation(Vector3::Up() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKeyDown(KeyCode::Space))
	{
		Instantiate<PlayerBullet>(pTransform_->position + Vector3::Down() * 1.0f, pTransform_->rotate);
	}
}

void PlayerGunner::Draw() const
{
}
