#include "Camera.h"

using namespace mtgb;

namespace
{
	static const float MOVE_SPEED{ 10.0f };
}

Camera::Camera(const Vector3& _position, mtgb::WindowContext _context) : GameObject(GameObjectBuilder()
		.SetName("Main Camera")
		.SetPosition(_position)
		.SetRotate(Quaternion::Identity())
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() }
{
	context_ = _context;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	const float MOVE{ MOVE_SPEED * Time::DeltaTimeF() };
	

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

void Camera::Draw() const
{
}
