#include "Bullet.h"


using namespace mtgb;

namespace
{
	static constexpr size_t BULLET_SPEED{ 1 };  // 1秒当たりの移動ピクセルスピード
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
	//hImage_ = Image::Load("Image/bullet.png");
	hModel_ = OBJ::Load("Model/OBJ/Bullet.obj");
	pRb_->velocity_ = { 0, 0, BULLET_SPEED };
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	//static int timer = 0;
	//timer += 1;
	//if (InputUtil::GetKeyDown(KeyCode::V) /*|| timer > 60 * 5*/)
	//{
	//	DestoryMe();
	//}

	//pTransform_->position_.z += 0.1f;
	//LOGF("bulletだよ imagehandle:%d\n", hImage_);
}

void Bullet::Draw() const
{
	//Draw::Image(hImage_, pTransform_);
	//Draw::OBJModel(hModel_, pTransform_);
}
