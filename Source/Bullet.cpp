#include "Bullet.h"


using namespace mtgb;

namespace
{
	const float BULLET_SPEED{ 10.f }; // 1秒当たりの移動ピクセルスピード
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
	LOGF("(%f, %f, %f)\n", _position.x, _position.y, _position.z);
	//hImage_ = Image::Load("Image/bullet.png");
	hModel_ = Fbx::Load("Model/gCube.fbx");
	pRb_->velocity_ = { 0, 0, 0 };
	pTransform_->scale_ = Vector3(1.f, 1.f, 1.f);
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
	pRb_->velocity_ = { 0, 0, BULLET_SPEED };
}

void Bullet::Draw() const
{

	Draw::FBXModel(hModel_, *pTransform_, 0);

}
