#include "EnemyBullet.h"

EnemyBullet::EnemyBullet(const Vector3& _position, const Quaternion& _quaternion) : GameObject(GameObjectBuilder()
	.SetPosition(_position)
	.SetRotate(_quaternion)
	.SetName("EnemyBullet")
	.Build()),
	pTransform_{ Component<Transform>() },
	pRigidBody_{ Component<RigidBody>() },
	pCollider_{ Component<Collider>() }
{
	hModel_ = Fbx::Load("Model/NewBullet.fbx");
	massert(hModel_ >= 0 && "’e‚Ìƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");

	hModelCollider_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(hModelCollider_ >= 0 && "SphereColliderƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");

	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->SetCenter(Vector3::Zero());
	pCollider_->SetRadius(0.5f);

	// 3•bŒo‚Á‚½‚çÁ‚·
	Timer::AddAram(3.0f, [this] { DestroyMe(); });
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Update()
{
	pRigidBody_->velocity_ = pTransform_->Forward() * 60.0f;
}

void EnemyBullet::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
}
