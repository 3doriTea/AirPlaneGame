#include "EnemyPlane.h"
#include "../TrailEmitterSystem.h"
#include "EnemyBullet.h"
#include "../PlayScene/EnemiesController.h"
#include "../Bullet.h"
#include "../PlayScene/QuotaGauge.h"

using namespace mtgb;

namespace
{
	TextHandle hText;
	const int HIT_DAMAGE{ 10 };
	const float BROKEN_DOWN_SPEED{ 30.0f };
	const float BROKEN_ROTATE_Z_SPEED_PER_SEC{ 3.0f };  // 墜落中のz軸回転(1秒間あたりの回転角度)
	const float DESTROY_HEIGHT{ -100 };  // 飛行機を消す高さ
	const float CHASE_SPEED{ 3.0f }; // ターゲットを追いかける速さ
	const float ENEMY_SCALE{ 0.5f }; // スケール
	const float SHOOT_COOLDOWN{ 5.0f }; // 弾を撃つクールダウン時間
	const int MAX_BULLETS{ 3 }; // 同時に存在できる弾の最大数
	const int ENEMY_PLANE_SCORE{ 100 }; // 倒された際に得られるスコア

	// デフォルトの敵スピード
	const float DEFAULT_SPEED{ 10.0f };
	// デフォルトの耐久値
	const int DEFAULT_HP{ 100 };

	const float ONE_SHOT_TIME_SEC{ 1.0f };     // 1発撃ったあとの待機時間(秒)
	const float RELOAD_TIME_SEC{ 5.0f };      // リロード中の待機時間(秒)
	const int BULLET_COUNT{ 1 };          // リロードまでに撃てる弾数
	const float LOCK_ON_TIME_SEC{ 3.0f };		// ロックオンにかかる時間
	const float LOCK_ON_ANGLE{ 60 }; // ロックオン、プレイヤーを発見する角度(degree)
	const float LOCK_ON_DISTANCE_{200};// ロックオン、プレイヤーを発見する距離
	const float ROUND_SPEED{ 1.0f };  // 回転飛行中の1秒間あたりの回転角度
	const int ADD_QUOTA_POINT{ 2 };  // 撃破時に加算するポイント
}

EnemyPlane::EnemyPlane(
	const Vector3& _worldPosition,
	const EntityId _playerPlane,
	const EntityId _controllerId) : GameObject(GameObjectBuilder()
		.SetName("Enemy")
		.SetTag(GameObjectTag::Enemy)
		.SetPosition(_worldPosition)
		.SetScale({ ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE })
		.Build()),
	pRB_{ Component<RigidBody>() },
	pTransform_{ Component<Transform>() },
	pCollider_{ Component<Collider>() },
	//pCollider_{ Component<Collider>()},
	pTarget_{ &Transform::Get(_playerPlane) },
	controllerId_{ _controllerId },
	speed_{ 10.0f },
	health_{},
	targetingSystem_{ pTransform_, RayDetectorConfig{
		.base = 
		{
			.targetTag = GameObjectTag::PlayerPlane,
			.maxDistance = LOCK_ON_DISTANCE_,
			.minDistance = 0.0f,
		},
		.rayTransform = pTransform_,
		.maxAngleDegrees = LOCK_ON_ANGLE,
	} },
	gun_
	{
		Gun::Setting  // 銃器の設定
		{
			.lockOnTimeSec = LOCK_ON_TIME_SEC,
			.oneShotTimeSec = ONE_SHOT_TIME_SEC,
			.reloadTimeSec = RELOAD_TIME_SEC,
			.bulletCount = BULLET_COUNT,
			.shooterType = ProjectTile::Shooter::Enemy,
			.projectileType = ProjectTile::Type::Missile,  // ミサイルを使用
		}
	},
	ai_{}
{
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->SetCenter(Vector3::Zero());
	pCollider_->SetRadius(2.0f);
	timeSinceLastshot_ = 0.0f;
	
	hModel_ = Fbx::Load("Model/Enemy01.fbx");
	massert(hModel_ >= 0 && "敵飛行機モデル読み込みに失敗");

	pRB_->OnCollisionEnter([this](EntityId _targetId)
		{
			if (broken_)
			{
				return;  // すでに壊れているなら当たっても処理することはない
			}
			GameObject* pTarget{ FindGameObject(_targetId) };
			if (pTarget == nullptr)
			{
				//LOGF("Id:%d(壁)と衝突した！ by %d(%s)\n", _targetId,entityId_, GetName().c_str());
				return;
			}

			if (pTarget->GetName() == "Bullet" || pTarget->GetName() == "Missile")
			{
				ProjectTile* pProjectile{ dynamic_cast<ProjectTile*>(pTarget) };

				if (pProjectile == nullptr)
				{
					return;
				}

				if (pProjectile->GetShooter() != ProjectTile::Shooter::Player)
				{
					return;
				}

				pTarget->DestroyMe();
				health_.Damage(HIT_DAMAGE);
				if (health_.IsDead())
				{
					broken_ = true;  // 体力的に死んでいるなら飛行機を壊す
					SetName("EnemyBroken");
					Audio::PlayOneShotFile("Sound/Effect/boom.wav");

					QuotaGauge* pQuotaGauge{ FindGameObject<QuotaGauge>() };
					if (pQuotaGauge != nullptr)
					{
						pQuotaGauge->AddPoint(ADD_QUOTA_POINT);
					}
				}
				else
				{
					Audio::PlayOneShotFile("Sound/Effect/ricochet.wav");
				}
			}
		});

	pEnemiesController_ = dynamic_cast<EnemiesController*>(FindGameObject(controllerId_));
	if (pEnemiesController_ == nullptr)
	{
		massert(false && "pEnemiesControllerが見つかりませんでした。 @Enemy::Update");
		return;
	}
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	
	ai_.SetInputData(
		{
			.playerPos = pEnemiesController_->GetPlayerPosition(),
			.pSelfTrans = pTransform_,
			.isBroken = broken_
		});

	ai_.Update();
	const EnemyAI::OutData& outData{ ai_.GetOutData() };

	if (HandleCrash())
	{
		return;
	}
	if (outData.isActive == false)
	{
		return;
	}

	if (outData.isAvoiding)
	{
		Audio::PlayOneShotFile("Sound/Effect/enemySwing.wav");
	}

	
	Fight(outData);
	

	Quaternion currentQua{ pTransform_->rotate };

	if (outData.isRound)
	{
		currentQua *= XMQuaternionRotationAxis(pTransform_->Forward(), Time::DeltaTimeF() * ROUND_SPEED);
	}

	Vector3 toPlayerDir{ outData.lookPosition - pTransform_->GetWorldPosition() };

	currentQua = Quaternion::SLerp(currentQua, Quaternion::LookRotation(toPlayerDir, Vector3::Up()), Time::DeltaTimeF() * 1.0f);

	// 前方向、頭は上方向に
	Vector3 forward{ pTransform_->Forward() };
	currentQua = Quaternion::SLerp(currentQua, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);

	pTransform_->rotate = currentQua;

	EntityId eId{ GetEntityId() };

	pRB_->velocity_ = pTransform_->Forward() * speed_;

		
	MTImGui::Instance().TypedShow(pTransform_, "EnemyPlane:" + std::to_string(entityId_));
	MTImGui::Instance().DirectShow([this]() 
		{
			ImGui::Text("LockOnProgress:%.3f" ,gun_.GetLockOnProgress());
			std::string hasTarget = targetingSystem_.HasTarget() ? "Yes" : "No";
			ImGui::Text("HasTarget:%s", hasTarget.c_str());
			
		}, "EnemyTargetingSystem:" + std::to_string(entityId_), ShowType::Inspector);
}

void EnemyPlane::Draw() const
{
	const EnemyAI::OutData& outData{ ai_.GetOutData() };
	if (status_.toDestroy_)
	{
		return;
	}
	Draw::FBXModel(hModel_, *pTransform_, 0);
	//pCollider_->Draw();
	Vector2Int pos = InputUtil::GetMousePosition();
}

void EnemyPlane::Fight(const EnemyAI::OutData& _outData)
{
	// 銃器の更新
	gun_.Update();
	if (_outData.isFire && ai_.GetMainState().Current() == EnemyAI::S_FIGHT)
	{
		// ターゲットを探す
		targetingSystem_.SearchTargets();
		// ターゲットがいるかどうか
		if (targetingSystem_.HasTarget())
		{
			// ロックオンを開始、更新
			gun_.StartLockOnCountdown();
			// ロックオンが完了したなら、射撃
			if (gun_.IsLockOnComplete())
			{
				gun_.Shot(pTransform_->GetWorldPosition(), pTransform_->rotate, pTarget_);
				gun_.ResetLockOnCountdown();

			}
		}	
	}
	// ターゲットがいないならロックオンを中止
	else
	{
		gun_.ResetLockOnCountdown();
	}
}

bool EnemyPlane::HandleCrash()
{
	if (broken_)  // 破壊中の処理
	{
		if (pTransform_->GetWorldPosition().y < DESTROY_HEIGHT)
		{
			// スコア加算
			Game::System<ScoreManager>().AddScore(ENEMY_PLANE_SCORE);
			DestroyMe();
			return true;
		}

		const float ROT_ANGLE{ Time::DeltaTimeF() * BROKEN_ROTATE_Z_SPEED_PER_SEC };
		Quaternion curr{ pTransform_->rotate };

		curr *= XMQuaternionRotationAxis((pTransform_->Right() + pTransform_->Forward()).Normalize(), ROT_ANGLE);

		Quaternion toLook{ Quaternion::FromToRotation(pTransform_->Forward(), Vector3::Down()) };
		pTransform_->rotate = Quaternion::SLerp(curr, curr * toLook, Time::DeltaTimeF());
		pRB_->velocity_ = pTransform_->Forward() * BROKEN_DOWN_SPEED;

		return true;
	}
	return false;
}

bool EnemyPlane::IsActive() const
{
	const EnemyAI::OutData& outData{ ai_.GetOutData() };
	return outData.isActive;
}
