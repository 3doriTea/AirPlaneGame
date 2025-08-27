#include "EnemyPlane.h"
#include "../TrailEmitterSystem.h"

using namespace mtgb;

namespace
{
	TextHandle hText;
	const int HIT_DAMAGE{ 10 };
	const float BROKEN_DOWN_SPEED{ 30.0f };
	const float BROKEN_ROTATE_Z_SPEED_PER_SEC{ 3.0f };  // 墜落中のz軸回転(1秒間あたりの回転角度)
	const float DESTROY_HEIGHT{ -100 };  // 飛行機を消す高さ
}

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
	speed_{ 10.0f },
	health_{},
	lockOnAngle_{ 45.0f },
	lockOnDistance_{ 30.0f }
{
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->sphere_.offset_ = Vector3::Zero();
	pCollider_->sphere_.radius_ = 1.0f;
	
	
	//hText = Text::Load("apple", 72);
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
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
	//Vector3 diffDir{ pTarget_->position - pTransform_->position };
	//DirectX::XMQuaternionBaryCentric
	/*Vector3 diffDir{ pTarget_->position - pTransform_->position };
	Quaternion lookQuaternion{ Quaternion::LookRotation(diffDir, pTransform_->Up()) };*/
	//pTransform_->rotate = Quaternion::LookRotation(Vector3::Normalize(pTarget_->position - pTransform_->position),Vector3::Up() );


	Search();

	MTImGui::Instance().TypedShow(pTransform_, "EnemyPlane:" + std::to_string(entityId_));
	MTImGui::Instance().DrawVec(pTransform_->position, pTransform_->Forward() * speed_, 2.0f);
}

void EnemyPlane::Draw() const
{
	//Draw::SetShaderOnce(ShaderType::Unlit3D);
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
	Vector2Int pos = InputUtil::GetMousePosition();
	
	/*Draw::ChangeTextAlignment(TextAlignment::center);
	Draw::Text(hText, 0, 0);
	Draw::ImmediateText("Banana", 0, 0, 72, TextAlignment::topLeft);*/
	//Draw::ImmediateText("hello world",0,0);
	//Game::System<ColliderCP>().TestDraw();
	
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
		LOGIMGUI("Enemy:%lld Lock On %.3f", entityId_,acosf(cosTheta));
		lockOnTarget_ = true;
	}
}