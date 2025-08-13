#include "EnemyPlane.h"

using namespace mtgb;

EnemyPlane::EnemyPlane(
	const Vector3& _worldPosition,
	const EntityId _playerPlane) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_worldPosition)
	.Build()),
	pRB_{ Component<RigidBody>() },
	pTransform_{ Component<Transform>() },
	pCollider_{ Component<Collider>() },
	pTarget_{ &Transform::Get(_playerPlane) },
	speed_{ 10.0f }
{
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->sphere_.offset_ = Vector3::Zero();
	pCollider_->sphere_.radius_ = 1.0f;
	pTransformProxy_ = new TransformProxy(pTransform_);
	pTransformGuizmo_ = new TransformGuizmo(pTransform_);
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	massert(hModel_ >= 0 && "“G”òs‹@ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý‚ÉŽ¸”s");

	pRB_->OnCollisionEnter([this](EntityId _targetId)
		{
			LOGF("Id:%d(%s)‚ÆÕ“Ë‚µ‚½I by %d(%s)\n", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			if (FindGameObject(_targetId)->GetName() == "PlayerBullet")
			{
				//DestroyMe();
			}
		});
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	Vector3 diffDir{ pTarget_->position - pTransform_->position };
	//Quaternion lookQuaternion{ Quaternion::FromToRotation(pTransform_->Forward(), diffDir) };
	//DirectX::XMQuaternionBaryCentric
	/*Vector3 diffDir{ pTarget_->position - pTransform_->position };
	Quaternion lookQuaternion{ Quaternion::LookRotation(diffDir, pTransform_->Up()) };
	pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, lookQuaternion, Time::DeltaTimeF());
	
	pRB_->velocity_ = pTransform_->Forward() * speed_;*/
}

void EnemyPlane::Draw() const
{
	Draw::SetShaderOnce(ShaderType::Unlit3D);
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();

	//Game::System<ColliderCP>().TestDraw();
}
