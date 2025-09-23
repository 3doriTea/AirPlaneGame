#include "Runway.h"

using namespace mtgb;

namespace
{
	const float MOVE_SPEED{ 10.0f };
}

Runway::Runway(const Vector3 _position, const Quaternion _quaternion) : GameObject(GameObjectBuilder()
	.SetPosition(_position)
	.SetRotate(_quaternion)
	.Build()),
	pTransform_{ Component<Transform>() }
{
	hModel_ = Fbx::Load("Model/airport/airport.fbx");
}

Runway::~Runway()
{
}

void Runway::Update()
{
}

void Runway::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
