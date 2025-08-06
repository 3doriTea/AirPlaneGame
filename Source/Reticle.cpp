#include "Reticle.h"
#include "WindowContext.h"

using namespace mtgb;

namespace
{
	static const float RETICLE_SIZE{ 10.0f };  // レティクルのサイズ

	// レティクルの初期座標
	RectInt START_RETICLE_RECT{
		120,120,
		300,300
	};  // レティクルの矩形

	// レティクルの画像切り取り
	RectInt RETICLE_CUT{
		0,0,
		711, 712
	};  // レティクルの切り取り矩形
}

Reticle::Reticle() : GameObject(GameObjectBuilder()
	.SetName("Reticle")
	.SetRotate(Quaternion::Identity())
	.Build()),
	pTransform_{ Component<Transform>() },
	pCamera_{ nullptr}
{
	hImage_ = Image::Load("Image/Reticle.png");	
	reticlePos_ = START_RETICLE_RECT;
}

Reticle::Reticle(mtgb::WindowContext context) : Reticle()
{
	context_ = context;
}

Reticle::~Reticle()
{
}

void Reticle::Update()
{
	// Kキーを押したら右に行く
	if (InputUtil::GetKeyDown(KeyCode::K, context_))
	{
		reticlePos_.point.x += 10;
	}
}

void Reticle::Draw() const
{
	if (context_ == WindowContext::First)
	{
		Draw::Image(hImage_, reticlePos_, RETICLE_CUT, 0.0f);
	}
	else if (context_ == WindowContext::Second)
	{
		Draw::Image(hImage_, reticlePos_, RETICLE_CUT, 0.0f);
	}
}
