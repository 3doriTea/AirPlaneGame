#include "ResultLogo.h"

ResultLogo::ResultLogo() : GameObject(GameObjectBuilder()
	.SetName("Resultlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 3.0,3.0,3.0 })
	.Build()),
	pTransform_{ Component<Transform>() }
{

	fModel_ = Fbx::Load("Model/over_logo.test.fbx");
}

ResultLogo::ResultLogo(mtgb::WindowContext context) : ResultLogo()
{
	context_ = context;
}

ResultLogo::~ResultLogo()
{
}

void ResultLogo::Update()
{
}

void ResultLogo::Draw() const
{
	Draw::SetShaderOnce(ShaderType::Unlit3D);
	Draw::FBXModel(fModel_, *pTransform_, 300);
}
