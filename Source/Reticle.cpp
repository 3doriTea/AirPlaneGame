#include "Reticle.h"
#include "WindowContext.h"

using namespace mtgb;

namespace
{
	static const float RETICLE_SIZE{ 10.0f };  // ���e�B�N���̃T�C�Y

	// ���e�B�N���̏������W
	RectInt START_RETICLE_RECT{
		120,120,
		300,300
	};  // ���e�B�N���̋�`

	// ���e�B�N���̉摜�؂���
	RectInt RETICLE_CUT{
		0,0,
		711, 712
	};  // ���e�B�N���̐؂����`
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
	// K�L�[����������E�ɍs��
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
