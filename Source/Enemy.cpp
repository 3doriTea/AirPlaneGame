#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"
#include "PlayScene/EnemiesController.h"


namespace
{
	// デフォルトの敵スピード
	const float DEFAULT_SPEED{ 10.0f };
	// デフォルトの耐久値
	const int DEFAULT_HP{ 100 };
}

Enemy::Enemy(const Vector3& _position, const EntityId _controllerId) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_position)
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() },
	hp_{ DEFAULT_HP },
	radius_{ 30 },
	speed_{ DEFAULT_SPEED },
	pRigidBody_{ Component<RigidBody>() },
	controllerId_{ _controllerId }
{
	hModel_ = Fbx::Load("Model/Enemy01.fbx");
	massert(hModel_ >= 0 && "敵飛行機モデル読み込みに失敗");

	Vector3 worldPos{ pTransform_->GetWorldPosition() };
	LOGF("EnemySpawan(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);

	pEnemiesController_ = dynamic_cast<EnemiesController*>(FindGameObject(controllerId_));
	if (pEnemiesController_ == nullptr)
	{
		massert(false && "pEnemiesControllerが見つかりませんでした。 @Enemy::Update");
		return;
	}
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	ai_.SetInputData(
	{
		.playerPos = pEnemiesController_->GetPlayerPosition(),
		.pSelfTrans = pTransform_,
	});

	ai_.Update();
	const EnemyAI::OutData& outData{ ai_.GetOutData() };

	if (outData.isActive == false)
	{
		return;
	}

	if (outData.isFire)
	{
		// TODO: 撃つ
	}

	Quaternion currentQua{ pTransform_->rotate };

	Vector3 toPlayerDir{ outData.lookPosition - pTransform_->GetWorldPosition() };

	/*if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(toPlayerDir, pTransform_->Right())) < 0)
	{
		Quaternion rotate{ DirectX::XMQuaternionRotationRollPitchYaw(1, 0, 1) };
		currentQua = Quaternion::SLerp(currentQua, rotate, Time::DeltaTimeF());
	}
	else
	{
		Quaternion rotate{ DirectX::XMQuaternionRotationRollPitchYaw(1, 0, -1) };
		currentQua = Quaternion::SLerp(currentQua, rotate, Time::DeltaTimeF());
	}*/
	currentQua = Quaternion::SLerp(currentQua, Quaternion::LookRotation(toPlayerDir, Vector3::Up()), Time::DeltaTimeF());


	// 前方向、頭は上方向に
	Vector3 forward{ pTransform_->Forward() };
	currentQua = Quaternion::SLerp(currentQua, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);

	pTransform_->rotate = currentQua;

	pRigidBody_->velocity_ = pTransform_->Forward() * speed_;
}

void Enemy::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
