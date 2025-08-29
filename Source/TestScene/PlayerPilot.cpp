#include "PlayerPilot.h"
#include "PlayerBullet.h"
#include <algorithm>
#include "../TrailEmitterSystem.h"

using namespace mtgb;

namespace
{
	
}
PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })

	.Build()),
	pTransform{ Component<Transform>() },
	lockOnSide_{400.0f},
	enemyFrameSideExtents_{30.0f},
	lockOnAny_{ false },
	lockOnDistance_{30.0f},
	pTargetInfo_{nullptr}
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();

	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	
	lockOnRect_ = 
	{
		rectCenter.x - lockOnSide_ / 2.0f,
		rectCenter.y - lockOnSide_ / 2.0f,
		lockOnSide_,
		lockOnSide_ 
	};


	rectDetector.config = 
	{
		.targetName = "Enemy",
		.windowContext = WindowContext::First,
		.detectionRect
			{
				rectCenter.x - lockOnSide_ / 2.0f,
				rectCenter.y - lockOnSide_ / 2.0f,
				lockOnSide_,
				lockOnSide_
			},
		.maxDistance = 300.0f,
	};

	pTransform->SetParent(_plane);

	lockOnFrame_ = Image::Load("Image/lockOnFrame.png");
	lockOnReticle_ = Image::Load("Image/lockOnReticle.png");

	enemyFrameRect_.size = { enemyFrameSideExtents_ * 2.0f,enemyFrameSideExtents_ * 2.0f};
	
	uiParams_.layerFlag = GameObjectLayer::A;
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
	LockOn();
	if (InputUtil::GetKeyDown(KeyCode::Space) || InputUtil::GetGamePadDown(FlightStickCode::Thumb))
	{
		LOGIMGUI("Pilot:shoot");
		Shoot();
	}
	
	MTImGui::Instance().DirectShow([]() 
		{
			float xAxis	 = InputUtil::GetAxis(Axis::X,WindowContext::First);
			float yAxis  = InputUtil::GetAxis(Axis::Y,WindowContext::First);
			ImGui::Text("x,y(%.3f,%.3f)", xAxis, yAxis);
		},"PilotJoystick",ShowType::Inspector);
	MTImGui::Instance().TypedShow(pTransform, "PlayerPilot");
}

void PlayerPilot::Draw() const
{
	//Ž©“®‚Å‘_‚¢‚ð’è‚ß‚é”ÍˆÍ‚ð•`‰æ
	const Vector2Int DRAW_SIZE{ lockOnSide_ ,lockOnSide_ };
	Draw::Image(lockOnFrame_, lockOnRect_,uiParams_);

	//‘_‚¢‚ª’è‚Ü‚Á‚Ä‚¢‚é“G‚ð‹­’²•\Ž¦
	if (rectDetector.HasDetectedTargets())
	{
		Draw::Image(lockOnReticle_, enemyFrameRect_, uiParams_);
	}

	Game::System<TrailEmitterSystem>().Render();
}

void PlayerPilot::LockOn()
{
	rectDetector.UpdateDetection();

	// ƒ[ƒ‹ƒhÀ•WŒn‚Åˆê”Ô‹ß‚¢“G‚ð‘_‚¤

	auto it = std::min_element(
		rectDetector.detectedTargets.begin(),
		rectDetector.detectedTargets.end(),
		[this](const RectContainsInfo& a, const RectContainsInfo& b)
		{
			float da = (pTransform->position - a.worldPos).Size();
			float db = (pTransform->position - b.worldPos).Size();
			return da < db;
		}
	);
	
	if (it != rectDetector.detectedTargets.end())
	{
		pTargetInfo_ = &(*it); // ƒAƒhƒŒƒX‚ð‘ã“ü
		enemyFrameRect_.x = pTargetInfo_->screenPos.x - enemyFrameSideExtents_ ;
		enemyFrameRect_.y = pTargetInfo_->screenPos.y - enemyFrameSideExtents_ ;
	}
	else
	{
		pTargetInfo_ = nullptr; // Œ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í nullptr
	}

}

void PlayerPilot::Shoot()
{
	if (rectDetector.HasDetectedTargets())
	{
		Vector3 toTarget = Vector3::Normalize(pTargetInfo_->worldPos - pTransform->position);
		Quaternion shootDir = Quaternion::LookRotation(toTarget, Vector3::Up());
		Instantiate<PlayerBullet>(pTransform->position + Vector3::Forward() * 1.0f, shootDir);
	}
}
