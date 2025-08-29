#include "PlayerPilot.h"
#include "PlayerBullet.h"
#include <algorithm>
#include "../TrailEmitterSystem.h"
#include "../TargetingSystem.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.Build()),
	pTransform{ Component<Transform>() }
{
	pTransform->SetParent(_plane);

	Vector2Int screenSize = Game::System<Screen>().GetSize();
	Vector2F rectCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
	float lockOnSide = 400.0f;

	// TargetingSystem‚ð‰Šú‰»
	pTargetingSystem_ = new TargetingSystem();
	pTargetingSystem_->Initialize(pTransform, rectCenter, lockOnSide);
	pTargetingSystem_->targetDetector.config.windowContext = WindowContext::First;
	pTargetingSystem_->uiParams.layerFlag = GameObjectLayer::A;
}

PlayerPilot::~PlayerPilot()
{
	delete pTargetingSystem_;
}

void PlayerPilot::Update()
{
	pTargetingSystem_->SearchTargets();
	if (InputUtil::GetKeyDown(KeyCode::Space) || InputUtil::GetGamePadDown(FlightStickCode::Thumb,WindowContext::First))
	{
		pTargetingSystem_->FireAtTarget();
	}
	
	MTImGui::Instance().DirectShow([]() 
		{
			float xAxis	 = InputUtil::GetAxis(Axis::X, WindowContext::First);
			float yAxis  = InputUtil::GetAxis(Axis::Y, WindowContext::First);
			ImGui::Text("x,y(%.3f,%.3f)", xAxis, yAxis);
		},"PilotJoystick", ShowType::Inspector);
	MTImGui::Instance().TypedShow(pTransform, "PlayerPilot");
}

void PlayerPilot::Draw() const
{
	pTargetingSystem_->DrawUI();

	Game::System<TrailEmitterSystem>().Render();
}


