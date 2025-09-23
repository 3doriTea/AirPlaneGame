#include "PlayerGunner.h"
#include "PlayerBullet.h"
#include "UI/Radar.h"
#include "../TargetingSystem.h"
#include "PlayerGun.h"

using namespace mtgb;

namespace
{
	const float ANGLE_X_MIN{ -DirectX::XM_PIDIV2 + 0.1f };
	const float ANGLE_X_MAX{ DirectX::XM_PIDIV2 - 0.1f };
}

PlayerGunner::PlayerGunner(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.SetTag(GameObjectTag::Player)
	.Build()),
	pTransform_{ Component<Transform>() },
	angleX_{ 0.0f },
	angleY_{ 0.0f },
	pRadarUI_{ nullptr },
	pPlaneTransform_{ &Transform::Get(_plane) }
{
	// PlayerGunnerが乗る飛行機のTransformは親に設定しない
	//pTransform_->SetParent(_plane);

	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = 100.0f;

	pPlayerGun_ = new PlayerGun(WindowContext::Second, GameObjectLayer::B, pTransform_, lockOnSide);

	// Raderを初期化
	pRadarUI_ = Instantiate<Radar>(entityId_, GameObjectLayer::B);
}

PlayerGunner::~PlayerGunner()
{
	delete pPlayerGun_;
}

void PlayerGunner::Update()
{
	pPlayerGun_->Update();

	// 位置を飛行機に同期させる
	pTransform_->position = pPlaneTransform_->GetWorldPosition();

	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };
#if 1
	Vector2F axis = InputUtil::GetAxis(WindowContext::Second);
	// 上
	if (axis.y > 0)
	{
		angleX_ += axis.y * ANGLE_SPEED * Time::DeltaTimeF();
		if (angleX_ > ANGLE_X_MAX)
		{
			angleX_ = ANGLE_X_MAX;
		}
	}
	else if (axis.y < 0)
	{
		angleX_ += axis.y * ANGLE_SPEED * Time::DeltaTimeF();
		if (angleX_ < ANGLE_X_MIN)
		{
			angleX_ = ANGLE_X_MIN;
		}
	}
	// 右
	if (axis.x > 0)
	{
		angleY_ += axis.x * ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ < 0.0f)
		{
			angleY_ += DirectX::XM_2PI;
		}
	}
	else if (axis.x < 0)
	{
		angleY_ += axis.x * ANGLE_SPEED * Time::DeltaTimeF();
		if (angleY_ >= DirectX::XM_2PI)
		{
			angleY_ -= DirectX::XM_2PI;
		}
	}
#elif 1
	Vector2F axis = InputUtil::GetAxis(WindowContext::Second);
	// 上
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
	// 右
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

	// 前方向、頭は上方向に
	Vector3 forward{ Vector3::Forward() };
	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.001f);

	pTransform_->rotate = curr;

	pTransform_->rotate = Quaternion::Euler({ angleX_, angleY_, 0.0f });

	pPlayerGun_->Update();
	if (InputUtil::GetKey(KeyCode::Space) || InputUtil::GetGamePad(PadCode::RB,WindowContext::Second))
	{
		pPlayerGun_->Fire();
	}

	if (pRadarUI_)
	{
		using namespace DirectX;
		
		pRadarUI_->Update();
		Matrix4x4 mRotSelf{};
		pTransform_->GenerateWorldRotationMatrix(&mRotSelf);
		Matrix4x4 mRotPlane{};
		pPlaneTransform_->GenerateWorldRotationMatrix(&mRotPlane);

		Vector3 angles{ XMVector3AngleBetweenVectors(pPlaneTransform_->Forward(), pTransform_->Forward()) };

		Vector3 rightAngles{ XMVector3AngleBetweenVectors(pPlaneTransform_->Forward(), pTransform_->Right()) };

		float angle{ angles.y };
		
		if (rightAngles.y > XM_PIDIV2)
		{
			angle = XM_2PI - angle;
		}


		//angle = DirectX::XMVector3Dot(gunForward, planeForward).m128_f32[0];

		//DirectX::XMQuaternionToAxisAngle(reinterpret_cast<DirectX::XMVECTOR*>(&pTransform_->rotate), &angle, Vector3::Up());
		pRadarUI_->SetViewAngle(angle);
	}

}

void PlayerGunner::Draw() const
{
//	pTargetingSystem_->DrawUI();
	pPlayerGun_->Draw();
	/*if (pRadarUI_)
	{
		pRadarUI_->Draw();
	}*/
}
