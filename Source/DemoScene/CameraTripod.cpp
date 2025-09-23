#include "CameraTripod.h"

CameraTripod::CameraTripod(const Vector3 _position, const Quaternion _quaternion) :
	GameObject(GameObjectBuilder()
		.SetPosition(_position)
		.SetRotate(_quaternion)
		.Build())
{
}

CameraTripod::~CameraTripod()
{
}

void CameraTripod::Update()
{
}

void CameraTripod::Draw() const
{
}
