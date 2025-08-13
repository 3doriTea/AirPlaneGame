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

	if (InputUtil::GetKey(KeyCode::Up))
	{
		pTransform_->Rotation(Vector3::Right());
	}
	if (InputUtil::GetKey(KeyCode::Down))
	{
		pTransform_->Rotation(Vector3::Left());
	}
	if (InputUtil::GetKey(KeyCode::Left))
	{
		pTransform_->Rotation(Vector3::Up());
	}
	if (InputUtil::GetKey(KeyCode::Right))
	{
		pTransform_->Rotation(Vector3::Down());
	}
	Matrix4x4 world{};
	Vector3 worldPos{};
	//pRB_->velocity_ = (pTransform_->Forward()) * Time::DeltaTimeF();
}

void PlayerPlane::Draw() const
{
}
