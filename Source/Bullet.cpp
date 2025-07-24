#include "Bullet.h"


using namespace mtgb;

namespace
{
	static constexpr size_t BULLET_SPEED{ 60 };  // 1秒当たりの移動ピクセルスピード
}

Bullet::Bullet(const Vector3& _position) : GameObject(GameObjectBuilder()
		.SetName("bullet")
		.SetPosition(_position)
		.SetRotate({})
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() },
	pRb_{ Component<RigidBody>() }
{
	hImage_ = Image::Load("Image/bullet.png");
	pRb_->velocity_ = { 0, BULLET_SPEED, 0 };
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	if (InputData::GetKeyDown(KeyCode::V))
	{
		DestoryMe();
	}
	//LOGF("bulletだよ imagehandle:%d\n", hImage_);
}

void Bullet::Draw() const
{
	Draw::Image(hImage_, pTransform_);
}
