#include "Camera.h"

using namespace mtgb;

Camera::Camera() : GameObject(GameObjectBuilder()
		.SetName("Main Camera")
		.SetPosition(Vector3{ 0, 0, 0 })
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
	if (InputData::GetKey(KeyCode::Up))
	{
		pTransform_->position_.z += 1.0f;
	}
	if (InputData::GetKey(KeyCode::Down))
	{
		pTransform_->position_.z -= -1.0f;
	}
	if (InputData::GetKey(KeyCode::Left))
	{
		pTransform_->position_.x -= -1.0f;
	}
	if (InputData::GetKey(KeyCode::Right))
	{
		pTransform_->position_.x += -1.0f;
	}
}

void Camera::Draw() const
{
}
