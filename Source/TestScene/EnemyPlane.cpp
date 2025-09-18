#include "EnemyPlane.h"
#include "../TrailEmitterSystem.h"
#include "EnemyBullet.h"
#include "../PlayScene/EnemiesController.h"
#include "../Bullet.h"

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
	const float SHOOT_COOLDOWN{ 1.0f }; // 弾を撃つクールダウン時間
	const int MAX_BULLETS{ 5 }; // 同時に存在できる弾の最大数
	const int ENEMY_PLANE_SCORE{ 100 }; // 倒された際に得られるスコア

	// デフォルトの敵スピード
	const float DEFAULT_SPEED{ 10.0f };
	// デフォルトの耐久値
	const int DEFAULT_HP{ 100 };


	const float ONE_SHOT_TIME_SEC{ 1.0f };     // 1発撃ったあとの待機時間(秒)
	const float RELOAD_TIME_SEC{ 1.0f };      // リロード中の待機時間(秒)
	const int BULLET_COUNT{ 5 };          // リロードまでに撃てる弾数

	const float ROUND_SPEED{ 1.0f };  // 回転飛行中の1秒間あたりの回転角度
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
	pCollider_{ Component<Collider>()},
	//pCollider_{ Component<Collider>()},
	pTarget_{ &Transform::Get(_playerPlane) },
	controllerId_{ _controllerId },
	speed_{ 10.0f },
	health_{},
	lockOnAngle_{ 45.0f },
	lockOnDistance_{ 30.0f },
	gun_
	{
		Gun::Setting  // 銃器の設定
		{
			.oneShotTimeSec = ONE_SHOT_TIME_SEC,
			.reloadTimeSec = RELOAD_TIME_SEC,
			.bulletCount = BULLET_COUNT,
			.bulletType = ProjectTile::Shooter::Enemy,
			.projectileType = ProjectTile::Type::Missile,  // 通常弾を使用
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
			GameObject* pTarget{ FindGameObject(_targetId) };
			if (pTarget == nullptr)
			{
				LOGF("Id:%d(壁)と衝突した！ by %d(%s)\n", _targetId,entityId_, GetName().c_str());
				return;
			}
			LOGF("Id:%d(%s)と衝突した！ by %d(%s)\n", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			LOGIMGUI("Id:%d(%s)と衝突した！ by %d(%s)", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());

			//massert(pTarget != nullptr && "当たったが、相手のゲームオブジェクトが見つからなかった");
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
				}
			}
		});

	pEnemiesController_ = dynamic_cast<EnemiesController*>(FindGameObject(controllerId_));
	if (pEnemiesController_ == nullptr)
	{
		massert(false && "pEnemiesControllerが見つかりませんでした。 @Enemy::Update");
		return;
	}

	//Game::System<TrailEmitterSystem>().
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	if (broken_)  // 破壊中の処理
	{
		if (pTransform_->GetWorldPosition().y < DESTROY_HEIGHT)
		{
			// スコア加算
			Game::System<ScoreManager>().AddScore(ENEMY_PLANE_SCORE);
			DestroyMe();
			return;
		}

		const float ROT_ANGLE{ Time::DeltaTimeF() * BROKEN_ROTATE_Z_SPEED_PER_SEC };
		Quaternion curr{ pTransform_->rotate };

		curr *= XMQuaternionRotationAxis((pTransform_->Right() + pTransform_->Forward()).Normalize(), ROT_ANGLE);

		Quaternion toLook{ Quaternion::FromToRotation(pTransform_->Forward(), Vector3::Down())};
		pTransform_->rotate = Quaternion::SLerp(curr, curr * toLook, Time::DeltaTimeF());
		pRB_->velocity_ = pTransform_->Forward() * BROKEN_DOWN_SPEED;

		return;
	}

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
		//gun_.Shot(pTransform_->GetWorldPosition(), pTransform_->rotate);
		gun_.Shot(pTransform_->GetWorldPosition(), pTransform_->rotate, pTarget_);
	}

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

	if (lockOnTarget_)
	{
		/*Vector3 diffDir{ pTarget_->position - pTransform_->position };
		Quaternion lookQuaternion{ Quaternion::LookRotation(diffDir, pTransform_->Up()) };
		pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, lookQuaternion, Time::DeltaTimeF());
		pRB_->velocity_ = pTransform_->Forward() * CHASE_SPEED;*/
	}
	//Vector3 diffDir{ pTarget_->position - pTransform_->position };
	//DirectX::XMQuaternionBaryCentric

	Search();

	// もしターゲットしているなら、弾を打つ
	timeSinceLastshot_ += Time::DeltaTimeF();

	//std::vector<EnemyBullet*> bullets;
	//FindGameObjects<EnemyBullet>(&bullets);

	//if (lockOnTarget_ && timeSinceLastshot_ >= SHOOT_COOLDOWN)
	//{
	//	// 弾の数を制限して、弾の数が5以上の場合は撃たないようにする
	//	if (bullets.size() >= MAX_BULLETS)
	//	{
	//		return;
	//	}
	//	
	//	GameObject::Instantiate<EnemyBullet>(pTransform_->GetWorldPosition(), pTransform_->GetWorldRotate());
	//	timeSinceLastshot_ = 0.0f;
	//}

	MTImGui::Instance().TypedShow(pTransform_, "EnemyPlane:" + std::to_string(entityId_));
	MTImGui::Instance().DrawVec(pTransform_->position, pTransform_->Forward() * speed_, 2.0f);
}

void EnemyPlane::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
	Vector2Int pos = InputUtil::GetMousePosition();
	
}

void EnemyPlane::Search()
{
	Vector3 forward = pTransform_->Forward();
	Vector3 toTarget = pTarget_->position - pTransform_->position;
	float distance = toTarget.Size();

	// 内積
	float cosTheta = DirectX::XMVector3Dot(forward, Vector3::Normalize(toTarget)).m128_f32[0];

	// ロックオンする、視野に入っていると判定する角度のラジアン
	float lockOnAngleRadian = DirectX::XMConvertToRadians(lockOnAngle_);

	
	if (cosTheta > lockOnAngleRadian && distance <= lockOnDistance_ )
	{
		//LOGIMGUI("Enemy:%lld Lock On %.3f", entityId_,acosf(cosTheta));
		lockOnTarget_ = true;
	}
	else
	{
		lockOnTarget_ = false;
	}
}

bool EnemyPlane::LockOnTarget() const
{
	return lockOnTarget_;
}
