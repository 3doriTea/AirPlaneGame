#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"
#include "PlayScene/EnemiesController.h"


namespace
{
	// デフォルトの敵スピード
	const float DEFAULT_SPEED{ 2.0f };
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

	Vector3 toPlayerDir{ outData.lookPosition - pTransform_->GetWorldPosition() };
	pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, Quaternion::LookRotation(toPlayerDir, Vector3::Up()), 0.1f);

	pRigidBody_->velocity_ = pTransform_->Forward() * speed_;
}

void Enemy::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
