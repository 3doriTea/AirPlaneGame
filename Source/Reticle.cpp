#include "Reticle.h"

using namespace mtgb;

namespace
{
	static const float RETICLE_SIZE{ 10.0f };  // ���e�B�N���̃T�C�Y

	static const Vector3 START_RETICLE_POSITION{
		400.0f, 300.0f, 0.0f
	};  // ���e�B�N���̈ʒu
	static const int PATH_MAX = 32;

	// ���e�B�N���̏������W
	RectInt START_RETICLE_RECT{
		120,120,
		300,300
	};  // ���e�B�N���̋�`

	// ���e�B�N���̉摜�؂���
	RectInt START_RETICLE_CUT{
		0,0,
		711, 712
	};  // ���e�B�N���̐؂����`
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
	const Vector2Int IMAGE_SIZE{ Image::GetSize(hImage_) };
	const Vector2Int DRAW_SIZE{ 300, 300 };
	const Vector2Int DRAW_POSITION{  IMAGE_SIZE / -2 };

	Draw::Image(hImage_, { DRAW_POSITION, DRAW_SIZE }, { Vector2Int::Zero(), IMAGE_SIZE }, 0.0f);
}
