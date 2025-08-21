#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const Vector3& _position, const Quaternion& _quaternion) : GameObject(GameObjectBuilder()
	.SetPosition(_position)
	.SetRotate(_quaternion)
	.SetName("PlayerBullet")
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
	pCollider_->sphere_.offset_ = Vector3::Zero();
	pCollider_->sphere_.radius_ = 0.5f;

	// 3•bŒo‚Á‚½‚çÁ‚·
	Timer::AddAram(1.0f, [this] { DestroyMe(); });
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Update()
{
	pRigidBody_->velocity_ = pTransform_->Forward() * 100.0f;
}

void PlayerBullet::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
	//Draw::SetShaderOnce(ShaderType::Debug3D);
	//Draw::FBXModel(hModelCollider_, *pTransform_, 0);
}
