#include "PlayerGunner.h"
#include "PlayerBullet.h"
#include "UI/Radar.h"
#include "../TargetingSystem.h"
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
	pPlaneTransform_{ &Transform::Get(_plane) },
	pTargetingSystem_{}
{
	pTransform_->SetParent(_plane);

	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = 400.0f;

	// TargetingSystem‚ð‰Šú‰»
	pTargetingSystem_ = new TargetingSystem();
	pTargetingSystem_->Initialize(pTransform_, rectCenter, lockOnSide);
	pTargetingSystem_->targetDetector.config.windowContext = WindowContext::Second;
	pTargetingSystem_->uiParams.layerFlag = GameObjectLayer::B;
}

PlayerGunner::~PlayerGunner()
{
	delete pTargetingSystem_;
}

void PlayerGunner::Update()
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };

#if 1
	Vector2F axis = InputUtil::GetAxis(WindowContext::Second);
	// ã
	if (axis.y > 0)
	{
		angleX_ += ANGLE_SPEED * Time::DeltaTimeF();
		if (angleX_ > ANGLE_X_MAX)
		{
			angleX_ = ANGLE_X_MAX;
		}
	}
	else if (axis.y < 0)
		angleX_ -= ANGLE_SPEED * Time::DeltaTimeF();
	if (angleX_ < ANGLE_X_MIN)
	{
		angleX_ = ANGLE_X_MIN;
	}
	// ‰E
	if (axis.x > 0)
	{
		angleY_ += ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ < 0.0f)
		{
			angleY_ += DirectX::XM_2PI;
		}
	}
	else if (axis.x < 0)
	{
		angleY_ -= ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ >= DirectX::XM_2PI)
		{
			angleY_ -= DirectX::XM_2PI;
		}
	}
#else

	if (InputUtil::GetKey(KeyCode::W))
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
	}
#endif

	using DirectX::XMQuaternionRotationAxis;

	const float ROT_ANGLE{ Time::DeltaTimeF() };
	Quaternion curr{ pTransform_->rotate };

	// ‘O•ûŒüA“ª‚Íã•ûŒü‚É
	Vector3 forward{ Vector3::Forward() };
	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.001f);

	pTransform_->rotate = curr;

	pTransform_->rotate = Quaternion::Euler({ angleX_, angleY_, 0.0f });
	Vector3 worldPos{ pTransform_->GetWorldPosition() };
	Vector3 parentWorldPos{ pTransform_->GetParent()->GetWorldPosition() };
	//LOGF("G:Pos(%f, %f, %f)\n", pTransform_->position.x, pTransform_->position.y, pTransform_->position.z);
	//LOGF("G:Pos(%f, %f, %f)  pAA=(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z, parentWorldPos.x, parentWorldPos.y, parentWorldPos.z);
	//LOGF("G:Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);

	pTargetingSystem_->SearchTargets();
	if (InputUtil::GetKeyDown(KeyCode::Space) || InputUtil::GetGamePadDown(PadCode::RB,WindowContext::Second))
	{
		//Instantiate<PlayerBullet>(pTransform_->GetWorldPosition(), pTransform_->GetWorldRotate());
		pTargetingSystem_->FireAtTarget();
	}

	if (pRadarUI_)
	{
		float angle{};
		using namespace DirectX;

		Vector3 forward{ XMVector3Cross(pTransform_->Right(), Vector3::Up()) };

		angle = DirectX::XMVector3Dot(forward, pPlaneTransform_->Forward()).m128_f32[0];

		//DirectX::XMQuaternionToAxisAngle(reinterpret_cast<DirectX::XMVECTOR*>(&pTransform_->rotate), &angle, Vector3::Up());
		pRadarUI_->SetViewAngle(angle);
	}
	
}

void PlayerGunner::Draw() const
{
	pTargetingSystem_->DrawUI();
}