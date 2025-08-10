#include "EnemyPlane.h"

using namespace mtgb;

EnemyPlane::EnemyPlane(const Vector3& _worldPosition) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_worldPosition)
	.Build()),
	pRB_{ Component<RigidBody>() },
	pTransform_{ Component<Transform>() },
	speed_{ 10.0f }
{
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	massert(hModel_ >= 0 && "�G��s�@���f���ǂݍ��݂Ɏ��s");
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	Vector3 addAngle{ Vector3::One() * (DirectX::XMConvertToRadians(30.0f) * Time::DeltaTimeF()) };
	pTransform_->rotate += { addAngle.x, addAngle.y, addAngle.z, 0.0f };
	pRB_->velocity_ = pTransform_->Forward() * speed_;
}

void EnemyPlane::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
