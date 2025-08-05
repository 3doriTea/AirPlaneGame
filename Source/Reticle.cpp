#include "Reticle.h"

using namespace mtgb;

namespace
{
	static const float RETICLE_SIZE{ 10.0f };  // レティクルのサイズ

	static const Vector3 START_RETICLE_POSITION{
		400.0f, 300.0f, 0.0f
	};  // レティクルの位置
	static const int PATH_MAX = 32;
}

Reticle::Reticle() : GameObject(GameObjectBuilder()
	.SetName("Reticle")
	.SetPosition(START_RETICLE_POSITION)
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3(RETICLE_SIZE, RETICLE_SIZE, 1.0f))
	.Build()),
	pTransform_{ Component<Transform>() }
{
	hImage_ = Image::Load("Image/Reticle.png");
	char test[PATH_MAX]{};
	GetCurrentDirectory(PATH_MAX, test);
}

Reticle::~Reticle()
{
}

void Reticle::Update()
{
}

void Reticle::Draw() const
{
	Draw::Image(hImage_, pTransform_);
}
