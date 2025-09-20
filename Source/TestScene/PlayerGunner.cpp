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
//	pTargetingSystem_{}
{
	// PlayerGunnerが乗る飛行機のTransformは親に設定しない
	//pTransform_->SetParent(_plane);

	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = 200.0f;

	// TargetingSystemを初期化
	//pTargetingSystem_ = new TargetingSystem();
	//pTargetingSystem_->Initialize(pTransform_, rectCenter, lockOnSide);
	//pTargetingSystem_->targetDetector.config.windowContext = WindowContext::Second;
	//pTargetingSystem_->uiParams.layerFlag = GameObjectLayer::B;

	pPlayerGun_ = new PlayerGun(WindowContext::Second, GameObjectLayer::B, pTransform_, lockOnSide);

	// Raderを初期化
	pRadarUI_ = Instantiate<Radar>(_plane, GameObjectLayer::B);
}

PlayerGunner::~PlayerGunner()
{
//	delete pTargetingSystem_;
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

//	pTargetingSystem_->SearchTargets();
	pPlayerGun_->Update();
	if (InputUtil::GetKey(KeyCode::Space) || InputUtil::GetGamePad(PadCode::RB,WindowContext::Second))
	{
		//Instantiate<PlayerBullet>(pTransform_->GetWorldPosition(), pTransform_->GetWorldRotate());
	//	pTargetingSystem_->FireAtTarget();
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

		//mRot = mRot * XMMatrixRotationY(angleY_);

		//XMMatrixRotationY(angleY_);

		/*XMVector3Dot(pPlaneTransform_->Forward(), pTransform_->Forward())*/

		//XMVector3AngleBetweenVectors
		
		Vector3 angles{ XMVector3AngleBetweenVectors(pPlaneTransform_->Forward(), pTransform_->Forward()) };

		Vector3 rightAngles{ XMVector3AngleBetweenVectors(pPlaneTransform_->Forward(), pTransform_->Right()) };
		//angleQua *= mRotPlane * mRotSelf;

		float angle{ angles.y };
		
		if (rightAngles.y > XM_PIDIV2)
		{
			angle = XM_2PI - angle;
		}

		// Vector3 gunForward{ XMVector3Cross(pTransform_->Right(), Vector3::Up()) };
		// Vector3 planeForward{ XMVector3Cross(pPlaneTransform_->Right(), Vector3::Up()) };

		//angle = DirectX::XMVector3Dot(gunForward, planeForward).m128_f32[0];

		LOGF("angle=%2.0f, rightAngle=%2.0f\n", XMConvertToDegrees(angles.y), XMConvertToDegrees(rightAngles.y));
		//DirectX::XMQuaternionToAxisAngle(reinterpret_cast<DirectX::XMVECTOR*>(&pTransform_->rotate), &angle, Vector3::Up());
		pRadarUI_->SetViewAngle(angle);
	}

	/*MTImGui::Instance().DirectShow([this]()
		{
			auto& targets =pTargetingSystem_->targetDetector.detectedTargets;
			for (RectContainsInfo& info : targets)
			{
				ImGui::Text("%.3f,%.3f", info.screenPos.x, info.screenPos.y);
			}
		},"GunnerContains",ShowType::Inspector);*/
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
