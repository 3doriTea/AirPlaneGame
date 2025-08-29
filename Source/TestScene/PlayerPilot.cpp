#include "PlayerPilot.h"
#include "PlayerBullet.h"
#include <algorithm>
#include "../TrailEmitterSystem.h"
#include "LockOn.h"

using namespace mtgb;

namespace
{
	
}
PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })

	.Build()),
	pTransform{ Component<Transform>() },
	lockOn_(),
	lockOnAny_{ false },
	lockOnDistance_{100.0f},
	pTargetInfo_{nullptr}
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();

	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	


	lockOn_->lockOnSide = 400.0f ;
	lockOn_->reticleSideExtents= 30.0f ;
	
	lockOn_->rectDetector.config =
	{
		.targetName = "Enemy",
		.windowContext = WindowContext::First,
		.detectionRect
			{
				rectCenter.x - lockOn_->lockOnSide / 2.0f,
				rectCenter.y - lockOn_->lockOnSide / 2.0f,
				lockOn_->lockOnSide,
				lockOn_->lockOnSide
			},
		.maxDistance = 300.0f,
	};

	pTransform->SetParent(_plane);

	lockOn_->frameImage = Image::Load("Image/lockOnFrame.png");
	lockOn_->reticleImage = Image::Load("Image/lockOnReticle.png");

	
	lockOn_->reticleRect.size = { lockOn_->reticleSideExtents * 2.0f,lockOn_->reticleSideExtents * 2.0f};
	
	lockOn_->uiParams.layerFlag = GameObjectLayer::A;
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
	LockOn();
	if (InputUtil::GetKeyDown(KeyCode::Space) || InputUtil::GetGamePadDown(FlightStickCode::Thumb,WindowContext::First))
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
	/*const Vector2Int DRAW_SIZE{ lockOnSide_ ,lockOnSide_ };
	Draw::Image(lockOnFrame_, lockOnRect_,lockOn_->uiParams);*/

	//‘_‚¢‚ª’è‚Ü‚Á‚Ä‚¢‚é“G‚ð‹­’²•\Ž¦
	if (rectDetector.HasDetectedTargets())
	{
		Draw::Image(lockOnReticle_, lockOn_->reticleRect, lockOn_->uiParams);
	}

	Game::System<TrailEmitterSystem>().Render();
}

//void PlayerPilot::LockOn()
//{
//	rectDetector.UpdateDetection();
//
//	// ƒ[ƒ‹ƒhÀ•WŒn‚Åˆê”Ô‹ß‚¢“G‚ð‘_‚¤
//
//	auto it = std::min_element(
//		rectDetector.detectedTargets.begin(),
//		rectDetector.detectedTargets.end(),
//		[this](const RectContainsInfo& a, const RectContainsInfo& b)
//		{
//			float da = (pTransform->position - a.worldPos).Size();
//			float db = (pTransform->position - b.worldPos).Size();
//			return da < db;
//		}
//	);
//	
//	if (it != rectDetector.detectedTargets.end())
//	{
//		pTargetInfo_ = &(*it); // ƒAƒhƒŒƒX‚ð‘ã“ü
//		lockOn_->reticleRect.x = pTargetInfo_->screenPos.x - enemyFrameSideExtents_ ;
//		lockOn_->reticleRect.y = pTargetInfo_->screenPos.y - enemyFrameSideExtents_ ;
//	}
//	else
//	{
//		pTargetInfo_ = nullptr; // Œ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í nullptr
//	}
//
//}

void PlayerPilot::Shoot()
{
	if (rectDetector.HasDetectedTargets())
	{
		Vector3 toTarget = Vector3::Normalize(pTargetInfo_->worldPos - pTransform->GetWorldPosition());
		Quaternion shootDir = Quaternion::LookRotation(toTarget, Vector3::Up());
		Instantiate<PlayerBullet>(pTransform->GetWorldPosition() , shootDir);
	}
}
