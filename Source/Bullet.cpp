#include "Bullet.h"


using namespace mtgb;

namespace
{
	const float BULLET_SPEED{ 100.f }; // 1秒当たりの移動ピクセルスピード
}

Bullet::Bullet(const Vector3& _position, const Quaternion& _quaternion, const Type bulletType_) : GameObject(GameObjectBuilder()
		.SetName("bullet")
		.SetPosition(_position)
		.SetRotate(_quaternion)
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() },
	pRb_{ Component<RigidBody>() },
	pCollider_{ Component<Collider>() },
	type_{ bulletType_ }
{
	hModel_ = Fbx::Load("Model/NewBullet.fbx");
	massert(hModel_ >= 0 && "弾のモデルの読み込みに失敗");

	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->SetCenter(Vector3::Zero());
	pCollider_->SetRadius(0.5f);

	// 3秒経ったら消す
	Timer::AddAram(3.0f, [this] { DestroyMe(); });
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	pRb_->velocity_ = pTransform_->Forward() * BULLET_SPEED;
}

void Bullet::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
}
