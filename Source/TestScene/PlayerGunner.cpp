#include "PlayerGunner.h"
#include "PlayerBullet.h"
#include "UI/Radar.h"

using namespace mtgb;

namespace
{
	const float ANGLE_X_MIN{ -DirectX::XM_PIDIV2 + 0.1f };
	const float ANGLE_X_MAX{ DirectX::XM_PIDIV2 - 0.1f };
}

PlayerGunner::PlayerGunner(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.Build()),
	pTransform_{ Component<Transform>() },
	angleX_{ 0.0f },
	angleY_{ 0.0f },
	pRadarUI_{ nullptr },
	pPlaneTransform_{ &Transform::Get(_plane) }
{
	pTransform_->SetParent(_plane);
}

PlayerGunner::~PlayerGunner()
{
}

void PlayerGunner::Update()
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };

	/*if (InputUtil::GetKey(KeyCode::W))
	{
		angleX_ -= ANGLE_SPEED * Time::DeltaTimeF();
		if (angleX_ < ANGLE_X_MIN)
		{
			angleX_ = ANGLE_X_MIN;
		}
		
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		angleX_ += ANGLE_SPEED * Time::DeltaTimeF();
		if (angleX_ > ANGLE_X_MAX)
		{
			angleX_ = ANGLE_X_MAX;
		}
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		angleY_ -= ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ >= DirectX::XM_2PI)
		{
			angleY_ -= DirectX::XM_2PI;
		}
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		angleY_ += ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ < 0.0f)
		{
			angleY_ += DirectX::XM_2PI;
		}
	}*/

	using DirectX::XMQuaternionRotationAxis;

	const float ROT_ANGLE{ Time::DeltaTimeF() };
	Quaternion curr{ pTransform_->rotate };

	if (InputUtil::GetKey(KeyCode::W))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE);
		//pTransform_->Rotation(Vector3::Right(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE);
		//pTransform_->Rotation(Vector3::Right(), ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		curr *= XMQuaternionRotationAxis(Vector3::Up(), -ROT_ANGLE);
		pTransform_->Rotation(Vector3::Up(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		curr *= XMQuaternionRotationAxis(Vector3::Up(), ROT_ANGLE);
		pTransform_->Rotation(Vector3::Up(), ROT_ANGLE);
	}

	// ‘O•ûŒüA“ª‚Íã•ûŒü‚É
	Vector3 forward{ Vector3::Forward() };
	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.001f);


	//curr = RemoveZRotation(curr);

	pTransform_->rotate = curr;

	//LOGF("ANGLE(%f, %f)\n", angleX_, angleY_);

	//pTransform_->rotate = Quaternion::Euler({ angleX_, angleY_, 0.0f });
	if (InputUtil::GetKeyDown(KeyCode::Space))
	//if (InputUtil::GetMouseDown(MouseCode::Left))
	{
		Instantiate<PlayerBullet>(pTransform_->GetWorldPosition(), pTransform_->GetWorldRotate());
		LOGIMGUI("Gunner:shoot");
	}
	Vector3 worldPos{ pTransform_->GetWorldPosition() };
	Vector3 parentWorldPos{ pTransform_->GetParent()->GetWorldPosition() };
	//LOGF("G:Pos(%f, %f, %f)\n", pTransform_->position.x, pTransform_->position.y, pTransform_->position.z);
	//LOGF("G:Pos(%f, %f, %f)  pAA=(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z, parentWorldPos.x, parentWorldPos.y, parentWorldPos.z);
	//LOGF("G:Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);
	Vector3 worldDiff{ worldPos - parentWorldPos };
	//LOGF("DIFF(%f, %f, %f)\n", worldDiff.x, worldDiff.y, worldDiff.z);

	if (pRadarUI_)
	{
		float angle{};
		using namespace DirectX;

		Vector3 forward{ XMVector3Cross(pTransform_->Right(), Vector3::Up()) };
		//Vector3 

		angle = DirectX::XMVector3Dot(forward, pPlaneTransform_->Forward()).m128_f32[0];

		//DirectX::XMQuaternionToAxisAngle(reinterpret_cast<DirectX::XMVECTOR*>(&pTransform_->rotate), &angle, Vector3::Up());
		pRadarUI_->SetViewAngle(angle);
	}
	
	//MTImGui::Instance().TypedShow(pTransform_, "PlayerGunner");
}

void PlayerGunner::Draw() const
{
	
}

//void PlayerGunner::ShowImGui()
//{
//	ImGui::Text("%d,Gunner", entityId_);
//	Inspector::Instance().ShowInspector(pTransform_, "Gunner");
//}
