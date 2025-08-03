#include "Camera.h"

using namespace mtgb;

namespace
{
	static const float MOVE_SPEED{ 10.0f };
}

Camera::Camera(const Vector3& _position) : GameObject(GameObjectBuilder()
		.SetName("Main Camera")
		.SetPosition(_position)
		.SetRotate(Quaternion::Identity())
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() }
{
	
}

Camera::~Camera()
{
}

void Camera::Update()
{
	const float MOVE{ MOVE_SPEED * Time::DeltaTimeF() };

	if (InputData::GetKey(KeyCode::Up))
	{
		pTransform_->position_.z += MOVE;
	}
	if (InputData::GetKey(KeyCode::Down))
	{
		pTransform_->position_.z -= MOVE;
	}
	if (InputData::GetKey(KeyCode::Left))
	{
		pTransform_->position_.x -= MOVE;
	}
	if (InputData::GetKey(KeyCode::Right))
	{
		pTransform_->position_.x += MOVE;
	}
}

void Camera::Draw() const
{
}
