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
	.SetLayerFlag(GameObjectLayer::B)
	.Build()),
	pTransform_{ Component<Transform>() }
	//pCamera_{ nullptr}
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
	const Vector2Int IMAGE_SIZE{ Image::GetSize(hImage_) };
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	const Vector2Int DRAW_SIZE{ IMAGE_SIZE };
	const Vector2Int DRAW_POSITION{ SCREEN_SIZE / 2 - DRAW_SIZE / 2 };

	Draw::Image(hImage_, { DRAW_POSITION, DRAW_SIZE }, { Vector2Int::Zero(), IMAGE_SIZE }, 0.0f);
}
