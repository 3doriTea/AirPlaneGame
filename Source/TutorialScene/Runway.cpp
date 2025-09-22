#include "Runway.h"

using namespace mtgb;

Runway::Runway() : GameObject(GameObjectBuilder()
	.SetPosition({ 15, -20, 200 })
	.SetRotate(Quaternion::Euler({ 0, -DirectX::XM_PIDIV2, 0 }))
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
