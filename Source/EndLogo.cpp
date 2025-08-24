#include "EndLogo.h"

EndLogo::EndLogo() : GameObject(GameObjectBuilder()
	.SetName("endlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 1,1,1 })
	.Build()),
	pTransform_{ Component<Transform>() }
{
	fModel_ = Fbx::Load("Model/result_logo.fbx");
	pTransform_->position = Vector3(0, 0, 0);
	pTransform_->scale = Vector3(0.1, 0.1, 0.1);
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
