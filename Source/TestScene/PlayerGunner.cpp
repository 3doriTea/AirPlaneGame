#include "PlayerGunner.h"

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
	const float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };

	if (InputUtil::GetKey(KeyCode::W))
	{
		pTransform_->Rotation(Vector3::Right() * ANGLE_SPEED);
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		pTransform_->Rotation(Vector3::Left() * ANGLE_SPEED);
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		pTransform_->Rotation(Vector3::Down() * ANGLE_SPEED);
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		pTransform_->Rotation(Vector3::Up() * ANGLE_SPEED);
	}

	LOGF("x:%f, y:%f, z:%f, w:%f\n",
		pTransform_->rotate_.f[0],
		pTransform_->rotate_.f[1],
		pTransform_->rotate_.f[2],
		pTransform_->rotate_.f[3]);
}

void PlayerGunner::Draw() const
{
}
