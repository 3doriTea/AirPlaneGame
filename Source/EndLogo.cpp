#include "EndLogo.h"

EndLogo::EndLogo() : GameObject(GameObjectBuilder()
	.SetName("endlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 3.0,3.0,3.0 })
	.Build()),
	pTransform_{ Component<Transform>() }
{

	fModel_ = Fbx::Load("Model/over_logo.test.fbx");
}

EndLogo::EndLogo(mtgb::WindowContext context) : EndLogo()
{
	context_ = context;
}

EndLogo::~EndLogo()
{
}

void EndLogo::Update()
{
}

void EndLogo::Draw() const
{
	Draw::SetShaderOnce(ShaderType::Unlit3D);
	Draw::FBXModel(fModel_, *pTransform_, 300);
}
