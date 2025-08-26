#include "EndLogo.h"

EndLogo::EndLogo() : GameObject(GameObjectBuilder()
	.SetName("endlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 0.1,0.1,0.1 })
	.Build()),
	pTransform_{ Component<Transform>() }
{
	fModel_ = Fbx::Load("Model/over_logo.test.fbx");
}

EndLogo::~EndLogo()
{
}

void EndLogo::Update()
{
}

void EndLogo::Draw() const
{
	Draw::FBXModel(fModel_, *pTransform_, 300);
}
