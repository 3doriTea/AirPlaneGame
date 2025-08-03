#include "Camera.h"

using namespace mtgb;

namespace
{
	static const float MOVE_SPEED{ 10.0f };
}

Camera::Camera() : GameObject(GameObjectBuilder()
	.SetName("Main Camera")
	.SetPosition(Vector3{ 0, 0, 0 })
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() }
{

}

Camera::Camera(mtgb::WindowContext context)
	:Camera()
{
	context_ = context;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	const float MOVE{ MOVE_SPEED * Time::DeltaTimeF() };
	if (context_ == WindowContext::First)
	{

		if (InputUtil::GetKey(KeyCode::Up, context_))
		{
			pTransform_->position_.z += MOVE;
		}
		if (InputUtil::GetKey(KeyCode::Down, context_))
		{
			pTransform_->position_.z -= MOVE;
		}
		if (InputUtil::GetKey(KeyCode::Left, context_))
		{
			pTransform_->position_.x -= MOVE;
		}
		if (InputUtil::GetKey(KeyCode::Right, context_))
		{
			pTransform_->position_.x += MOVE;
		}
	}
	else
	{
		if (InputUtil::GetKey(KeyCode::W, context_))
		{
			pTransform_->position_.z += MOVE;
		}
		if (InputUtil::GetKey(KeyCode::S, context_))
		{
			pTransform_->position_.z -= MOVE;
		}
		if (InputUtil::GetKey(KeyCode::A, context_))
		{
			pTransform_->position_.x -= MOVE;
		}
		if (InputUtil::GetKey(KeyCode::D, context_))
		{
			pTransform_->position_.x += MOVE;
		}
	}


}

void Camera::Draw() const
{
}
