#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const Vector3& _position, const Quaternion& _quaternion) : GameObject(GameObjectBuilder()
	.SetPosition(_position)
	.SetRotate(_quaternion)
	.Build()),
	pTransform_{ Component<Transform>() },
	pRigidBody_{ Component<RigidBody>() },
	pCollider_{ Component<Collider>() }
{
	hModel_ = Fbx::Load("Model/bullet.fbx");
	massert(hModel_ >= 0 && "’e‚Ìƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");

	hModelCollider_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(hModelCollider_ >= 0 && "SphereColliderƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Update()
{
	pRigidBody_->velocity_ = pTransform_->Forward();
}

void PlayerBullet::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
	Draw::SetShaderOnce(ShaderType::Debug3D);
	Draw::FBXModel(hModelCollider_, *pTransform_, 0);
}
