#include "Missile.h"
#include "Debug.h"

using namespace mtgb;

namespace
{
	const float MISSILE_CHASE_SPEED{ 30.0f };
	const float MAX_TRACKING_ANGLE{ 60.0f }; // 追尾可能な最大角度(degree)
	const float TURN_RATE{ 2.0f }; // 旋回速度 (1秒間あたりの回転角度の倍率)
	const float DESTROY_TIME{ 10.0f }; // 破棄されるまでの時間
}

Missile::Missile(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter, Transform* _target)
	: ProjectTile(_position, _quaternion, _shooter, Type::Missile)
	, pTarget_{_target}
{
	SetName("Missile");
	hModel_ = Fbx::Load("Model/AIM120D.fbx");
	
	massert(hModel_ >= 0 && "ミサイルのモデルの読み込みに失敗");

	// 衝突処理
	pCollider_->SetRadius(3.0f);
	pRb_->OnCollisionEnter([this](EntityId _id)
		{
			GameObject* pTarget{ FindGameObject(_id) };
			if (pTarget == nullptr)
			{
				return;
			}
			
			if (pTarget->GetName() == "PlayerPlane")
			{
				DestroyMe();
				LOGIMGUI("Id:%d(%s)と衝突した！ by %d(%s)", _id, FindGameObject(_id)->GetName().c_str(), entityId_, GetName().c_str());
			}
			if (pTarget->GetName() == "Bullet")
			{
				DestroyMe();
				LOGIMGUI("Id:%d(%s)に撃ち落された！ by %d(%s)", _id, FindGameObject(_id)->GetName().c_str(), entityId_, GetName().c_str());
			}

		});

	Timer::AddAram(DESTROY_TIME, [this] { DestroyMe(); });

}

Missile::Missile(const Transform& _shooterTransform, Transform* _target, const Shooter _shooter)
	: Missile{_shooterTransform.position,_shooterTransform.rotate,_shooter,_target}
{
	pTransform_->scale = _shooterTransform.scale;
}

void Missile::Update()
{
	if (pTarget_ == nullptr)
	{
		// ターゲットがない場合は直進
		pRb_->velocity_ = pTransform_->Forward() * MISSILE_CHASE_SPEED;
		return;
	}
	// ターゲットへの方向ベクトルを計算
	Vector3 toTarget{ pTarget_->GetWorldPosition() - pTransform_->GetWorldPosition() };

	// 正面方向とターゲットへの内積を計算
	float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pTransform_->Forward(), Vector3::Normalize(toTarget)));
	// 角度を算出(ラジアンで)
	float angleRadian = std::acosf(std::clamp(dotProduct, -1.0f, 1.0f));
	// degreeに直す
	float angleDegrees = DirectX::XMConvertToDegrees(angleRadian);

	// 追尾可能な角度内かチェック
	if (angleDegrees <= MAX_TRACKING_ANGLE)
	{
		// 非常に近い場合の除算エラーを避ける
		if (toTarget.Size() > 0.1f) 
		{
			// 角度制限内なら追尾
			//Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget), Vector3::Up()) };
			Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget), pTransform_->Up()) };

			// 補間値tを計算
			float lerpFactor = TURN_RATE * Time::DeltaTimeF();
			pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, targetRotation, lerpFactor);
		}
	}

	// 前方向に移動
	pRb_->velocity_ = pTransform_->Forward() * MISSILE_CHASE_SPEED;
}

void Missile::Draw() const
{
	ProjectTile::Draw();
}
