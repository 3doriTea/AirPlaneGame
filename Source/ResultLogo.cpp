#include "ResultLogo.h"

namespace
{
	// •`‰æ”ÍˆÍ
	const RectF DRAW_RECT{ 453.0f, -80.0f, 1013.33f, 570.0f };
	const UIParams UI_PARAMS{};
}

ResultLogo::ResultLogo() : GameObject(GameObjectBuilder()
	.SetName("Resultlogo")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 3.0,3.0,3.0 })
	.Build()),
	pTransform_{ Component<Transform>() }
{
	//fModel_ = Fbx::Load("Model/result_logo.fbx");
	hImage_ = Image::Load("Image/result.png");
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
	//Draw::FBXModel(fModel_, *pTransform_, 300);
	Draw::Image(hImage_, GenDrawScreenFrom(DRAW_RECT), UI_PARAMS);
}
