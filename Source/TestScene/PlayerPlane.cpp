 #include "PlayerPlane.h"
#include "UI/Radar.h"

#include <cmath>
using namespace mtgb;

namespace
{
	float defaultSpeed = 3.0f;
	float speed = 5.0f;
	
	// 最高速度
	float maxSpeed = 15.0f;
}

#define __X m128_f32[0]
#define __Y m128_f32[1]
#define __Z m128_f32[2]

PlayerPlane::PlayerPlane() : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.SetName("PlayerPlane")
	.SetTag(GameObjectTag::PlayerPlane)
	.Build()),
	pTransform_{ Component<Transform>() },
	pRB_{ Component<RigidBody>() },
	pCollider_{Component<Collider>(Collider::ColliderTag::GAME_OBJECT)},
	vVPlayer_{}
{
	// デバッグのためにImGuiのゲーム画面でモデルを表示する
	layerFlag_ = GameObjectLayer::SceneView;
	hModel_ = Fbx::Load("Model/Enemy01.fbx");
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->SetCenter(Vector3::Zero());
	pCollider_->SetRadius(1.0f);

	pRB_->OnCollisionEnter([this](EntityId _targetId)
		{
			GameObject* pTarget{ FindGameObject(_targetId) };
			if (pTarget == nullptr)
			{
				//LOGF("Id:%d(壁)と衝突した！ by %d(%s)\n", _targetId, entityId_, GetName().c_str());
				return;
			}
			if (pTarget->GetName() == "Bullet" || pTarget->GetName() == "Missile")
			{
				LOGIMGUI("%sは%sをくらった!", GetName().c_str(), pTarget->GetName().c_str());
				ScoreManager::SubtractScore(100);
				return;
			}
			//LOGF("Id:%d(%s)と衝突した！ by %d(%s)\n", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
		});

	// Raderを初期化
	Instantiate<Radar>(entityId_, GameObjectLayer::A);
}

PlayerPlane::~PlayerPlane()
{
}

void PlayerPlane::Update()
{
	using DirectX::XMQuaternionRotationAxis;

	const float ROT_ANGLE{ Time::DeltaTimeF() };

	Quaternion curr{ pTransform_->rotate };

#if 1
	// WindowContextを直接指定しない方いい
	Vector2F axis = InputUtil::GetAxis(WindowContext::First);
	//if (axis.x != 0.0f || axis.y != 0.0f)
	{

		// 上
		if (axis.y > 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE);
		}
		else if (axis.y < 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE);
		}
		// 右
		if (axis.x > 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Up(), ROT_ANGLE);
		}
		else if (axis.x < 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Up(), -ROT_ANGLE);
		}

		// 前方向、頭は上方向に
		Vector3 forward{ pTransform_->Forward() };
		curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);
		pTransform_->rotate = curr;
	}
#else
	if (InputUtil::GetKey(KeyCode::W))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), ROT_ANGLE);
	}
	// 前方向、頭は上方向に
	Vector3 forward{ pTransform_->Forward() };
	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);
	pTransform_->rotate = curr;
#endif

	if (InputUtil::GetKeyDown(KeyCode::F))
	{
		vVPlayer_.Play(u8"正面に敵が102体います");
	}


	// トリガーの押し込み具合
	float triggerValue = InputUtil::GetTrigger(FlightStickAxisCode::Slider, WindowContext::First);
	triggerValue = -(triggerValue - 1.0f);
	
	// 速度の倍率を計算
	float speedRatio = defaultSpeed + (maxSpeed - defaultSpeed) * triggerValue;
	speed = std::lerp(speed, speedRatio, Time::DeltaTimeF());
	

	// 速度を反映
	pRB_->velocity_ = pTransform_->Forward() * speed;

	MTImGui::Instance().DirectShow([this]() {
		TypeRegistry::Instance().CallFunc(&pTransform_->position, "Position");
		TypeRegistry::Instance().CallFunc(&pTransform_->rotate, "Rotation");
		TypeRegistry::Instance().CallFunc(&pRB_->velocity_, "Velocity");
		ImGui::InputFloat("Speed", &speed);
		},"PlayerPlane", ShowType::Inspector);
}

void PlayerPlane::Draw() const
{
	// もし描画されない場合はlayerFlag_を確認

	Draw::FBXModel(hModel_, *pTransform_,0);
}

Quaternion PlayerPlane::RemoveZRotation(Quaternion _q) const
{
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMVector3Cross;
	using DirectX::XMQuaternionRotationMatrix;

	// 回転行列
	Matrix4x4 mRot{ XMMatrixRotationQuaternion(_q) };

	Vector3 vUp{ mRot.r[1] };  // 上方向のベクトル

	// z軸を0にした上方向ベクトル
	Vector3 projectedUp{ vUp.x, vUp.y, 0 };
	projectedUp.Normalize();

	// z軸+方向のベクトル
	Vector3 forward{ mRot.r[2] };

	Vector3 right{ XMVector3Cross(forward, projectedUp) };
	right.Normalize();

	Matrix4x4 mNewRot
	{
		DirectX::XMMATRIX
		{
			right,
			projectedUp,
			forward,
			{ 0, 0, 0, 1 }
		}
	};

	return XMQuaternionRotationMatrix(mNewRot);
}
