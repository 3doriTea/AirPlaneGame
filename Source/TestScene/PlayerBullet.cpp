#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const Vector3& _position, const Quaternion& _quaternion) : GameObject(GameObjectBuilder()
	.SetPosition(_position)
	.SetRotate(_quaternion)
	.Build()),
	pTransform_{ Component<Transform>() },
	pRigidBody_{ Component<RigidBody>() },
	pCollider_{ Component<Collider>() }
{
	hModel_ = Fbx::Load("Model/NewBullet.fbx");
	massert(hModel_ >= 0 && "�e�̃��f���̓ǂݍ��݂Ɏ��s");

	hModelCollider_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(hModelCollider_ >= 0 && "SphereCollider���f���̓ǂݍ��݂Ɏ��s");

	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->sphere_.offset_ = {};
	pCollider_->sphere_.radius_ = 0.1f;
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
