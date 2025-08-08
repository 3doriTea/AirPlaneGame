#include "PlayerPlane.h"

using namespace mtgb;

PlayerPlane::PlayerPlane() : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.Build()),
	pTransform_{ Component<Transform>() },
	pRB_{ Component<RigidBody>() }
{
}

PlayerPlane::~PlayerPlane()
{
}

void PlayerPlane::Update()
{

	/*if (InputUtil::GetKey(KeyCode::W))
	{
		pTransform_->Rotation(Vector3::Right());
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		pTransform_->Rotation(Vector3::Left());
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		pTransform_->Rotation(Vector3::Down());
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		pTransform_->Rotation(Vector3::Up());
	}*/

	pRB_->velocity_ = pTransform_->Forward() * Time::DeltaTimeF();
}

void PlayerPlane::Draw() const
{
}
