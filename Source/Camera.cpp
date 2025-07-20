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
}

void Camera::Draw() const
{
}
