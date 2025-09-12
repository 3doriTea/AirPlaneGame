#include "EnemyPlane.h"
#include "../TrailEmitterSystem.h"
#include "EnemyBullet.h"

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
}

EnemyPlane::EnemyPlane(
	const Vector3& _worldPosition,
	const EntityId _playerPlane) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_worldPosition)
	.SetScale({ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE})
	.Build()),
	pRB_{ Component<RigidBody>() },
	pTransform_{ Component<Transform>() },
	pCollider_{ Component<Collider>() },
	pTarget_{ &Transform::Get(_playerPlane) },
	speed_{ 10.0f },
	health_{},
	lockOnAngle_{ 45.0f },
	lockOnDistance_{ 30.0f }
{
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->sphere_.offset_ = Vector3::Zero();
	pCollider_->sphere_.radius_ = 1.0f;
	timeSinceLastshot_ = 0.0f;
	
	//hText = Text::Load("apple", 72);
	hModel_ = Fbx::Load("Model/Enemy01.fbx");
	massert(hModel_ >= 0 && "敵飛行機モデル読み込みに失敗");

	pRB_->OnCollisionEnter([this](EntityId _targetId)
		{
			LOGF("Id:%d(%s)と衝突した！ by %d(%s)\n", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			LOGIMGUI("Id:%d(%s)と衝突した！ by %d(%s)", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			GameObject* pTarget{ FindGameObject(_targetId) };

			massert(pTarget != nullptr && "当たったが、相手のゲームオブジェクトが見つからなかった");
			if (pTarget->GetName() == "PlayerBullet")
			{
				pTarget->DestroyMe();
				health_.Damage(HIT_DAMAGE);
				if (health_.IsDead())
				{
					broken_ = true;  // 体力的に死んでいるなら飛行機を壊す
					SetName("EnemyBroken");
				}
			}
		});

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

	std::vector<EnemyBullet*> bullets;
	FindGameObjects<EnemyBullet>(&bullets);

	if (lockOnTarget_ && timeSinceLastshot_ >= SHOOT_COOLDOWN)
	{
		// 弾の数を制限して、弾の数が5以上の場合は撃たないようにする
		if (bullets.size() >= MAX_BULLETS)
		{
			return;
		}
		
		GameObject::Instantiate<EnemyBullet>(pTransform_->GetWorldPosition(), pTransform_->GetWorldRotate());
		timeSinceLastshot_ = 0.0f;
	}

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
