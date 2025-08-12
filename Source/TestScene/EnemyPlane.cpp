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
	pTarget_{ &Transform::Get(_playerPlane) },
	speed_{ 10.0f }
{
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	massert(hModel_ >= 0 && "“G”òs‹@ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý‚ÉŽ¸”s");
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	//Vector3 addAngle{ Vector3::One() * (DirectX::XMConvertToRadians(30.0f) * Time::DeltaTimeF()) };
	//pTransform_->rotate *= Quaternion::Euler(addAngle);
	//LOGF("(%f, %f, %f, %f)\n", pTransform_->rotate.X(), pTransform_->rotate.Y(), pTransform_->rotate.Z(), pTransform_->rotate.W());
	//pTransform_->rotate = Quaternion::SLerp(pTransform_->Forward(), pTarget_->position - pTransform_->position, Time::DeltaTimeF());
	
	Vector3 diffDir{ pTarget_->position - pTransform_->position };
	//Quaternion lookQuaternion{ Quaternion::FromToRotation(pTransform_->Forward(), diffDir) };
	Quaternion lookQuaternion{ Quaternion::LookRotation(diffDir, pTransform_->Up()) };
	pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, lookQuaternion, Time::DeltaTimeF());
	//DirectX::XMQuaternionBaryCentric
	
	pRB_->velocity_ = pTransform_->Forward() * speed_;
}

void EnemyPlane::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
