#include "ResultLogo.h"

ResultLogo::ResultLogo() : GameObject(GameObjectBuilder()
	.SetName("Resultlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 3.0,3.0,3.0 })
	.Build()),
	pTransform_{ Component<Transform>() }
{
	fModel_ = Fbx::Load("Model/result_logo.fbx");
	pTransform_->position.y = 3.5f;
}

ResultLogo::ResultLogo(mtgb::WindowContext _context) : ResultLogo()
{
	context_ = _context;
}

ResultLogo::~ResultLogo()
{
}

void ResultLogo::Update()
{
}

void ResultLogo::Draw() const
{
	Draw::FBXModel(fModel_, *pTransform_, 300);
}
