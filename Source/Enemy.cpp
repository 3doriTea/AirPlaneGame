#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"
#include "PlayScene/EnemiesController.h"


namespace
{
	// デフォルトの敵スピード
	const float DEFAULT_SPEED{ 10.0f };
	// デフォルトの耐久値
	const int DEFAULT_HP{ 100 };
	

	const float ONE_SHOT_TIME_SEC{ 0.25f };     // 1発撃ったあとの待機時間(秒)
	const float RELOAD_TIME_SEC{ 1.0f };      // リロード中の待機時間(秒)
	const int BULLET_COUNT{ 5 };          // リロードまでに撃てる弾数

	const float ROUND_SPEED{ 1.0f };  // 回転飛行中の1秒間あたりの回転角度
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
	controllerId_{ _controllerId },
	gun_
	{
		Gun::Setting  // 銃器の設定
		{
			.oneShotTimeSec = ONE_SHOT_TIME_SEC,
			.reloadTimeSec = RELOAD_TIME_SEC,
			.bulletCount = BULLET_COUNT,
			.bulletType = Bullet::Type::Enemy,
		}
	}
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

	gun_.Update();
	if (outData.isFire)
	{
		gun_.Shot(pTransform_->GetWorldPosition(), pTransform_->rotate);
	}

	Quaternion currentQua{ pTransform_->rotate };

	if (outData.isRound)
	{
		currentQua *= XMQuaternionRotationAxis(pTransform_->Forward(), Time::DeltaTimeF() * ROUND_SPEED);
	}

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
	currentQua = Quaternion::SLerp(currentQua, Quaternion::LookRotation(toPlayerDir, Vector3::Up()), Time::DeltaTimeF() * 1.0f);


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
