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

	MTImGui::Instance().DirectShow([this]()
		{
			auto& targets = pTargetingSystem_->targetDetector.detectedTargets;
			for (RectContainsInfo& info : targets)
			{
				ImGui::Text("%.3f,%.3f", info.screenPos.x, info.screenPos.y);
			}
		}, "PilotContains", ShowType::Inspector);


}

void PlayerPilot::Draw() const
{
	pTargetingSystem_->DrawUI();
	//Draw::ImmediateText("apple", { 100,0 }, 30,TextAlignment::middleLeft,UIParams{.layerFlag = GameObjectLayer::A});
	Game::System<TrailEmitterSystem>().Render();
}


