#include "TestScene.h"
#include "../PlayScene.h"
#include "../Reticle.h"
#include "../SkySphere.h"
#include "../Enemy.h"

#include "PlayerPlane.h"
#include "PlayerGunner.h"
#include "PlayerPilot.h"
#include "EnemyPlane.h"
#include "Terrain.h"
#include "UI/Radar.h"
#include "HighlightEnemy.h"
#include "../ControlTower.h"
#include "../TestScene/UI/OrderText.h"
#include "../CheckTutorialScene/CheckTutorialScene.h"
#include "../TutorialScene/TutorialScene.h"
#include "../PlayScene/QuotaGauge.h"
#include "../OverScene.h"
#include "TerrainReader.h"
#include "TimeLimit.h"
#include "../PlayScene/EnemiesController.h"
#include "../ResultScene.h"
using namespace mtgb;

namespace mtgb
{
	//TerrainReader16* reader16;
	TerrainReader8* reader8;
	TimeLimit* timeLimit_;

}

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
	//delete reader16;
	delete reader8;
}

void TestScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();
	
	Game::System<ScoreManager>().ResetScore();
	Instantiate<SkySphere>();
	Instantiate<Terrain>();
	Instantiate<Reticle>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>() };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };

	PlayerPilot* pPilot{Instantiate<PlayerPilot>(eIdPlayer)};
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pPilot);

	PlayerGunner* pGunner{ Instantiate<PlayerGunner>(eIdPlayer) };
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pGunner);

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	EnemiesController* pEnemiesController{ Instantiate<EnemiesController>(pPlayerPlane->GetEntityId()) };
	pEnemiesController->Spawan({ 0, 0, 10 });
	pEnemiesController->Spawan({ 0, 0, 300 });
	pEnemiesController->Spawan({ 0, -100, 500 });
	pEnemiesController->Spawan({ 100, 0, 500 });
	pEnemiesController->Spawan({ 300, 0, 500 });
	pEnemiesController->Spawan({ 300, 0, 500 });

	//Instantiate<EnemyPlane>(Vector3{ 0, 0, 0 }, eIdPlayer);
	
	
	//Instantiate<OrderText>(eIdPlayer, GameObjectLayer::A);

	//Instantiate<Radar>(eIdPlayer, GameObjectLayer::A);

	/*Radar* pGunnerRader{ Instantiate<Radar>(eIdPlayer, GameObjectLayer::B) };
	pGunner->SetRadarUI(pGunnerRader);*/


	ControlTower* pControlTower{ Instantiate<ControlTower>() };
	pControlTower->SetControlTarget(pGunner->GetEntityId(), WindowContext::Second);
	pControlTower->SetControlTarget(pPilot->GetEntityId(), WindowContext::First);
	
	reader8 = new TerrainReader8();
	reader8->Initialize();

	Instantiate<QuotaGauge>();
	
	timeLimit_ = Instantiate<TimeLimit>(180.0f);
	timeLimit_->StartTimer();
	timeLimit_->RegisterOnEndTimerCallback([]()
		{
			Game::System<SceneSystem>().Move<OverScene>();
		});
}

void TestScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<ResultScene>();
	}
	if (InputUtil::GetKeyDown(KeyCode::Y))
	{
		Game::System<SceneSystem>().Move<CheckTutorialScene>();
	}
	if (InputUtil::GetKeyDown(KeyCode::U))
	{
		Game::System<SceneSystem>().Move<TutorialScene>();
	}
	
	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}
}

void TestScene::Draw() const
{
	reader8->TestDraw();
	
	
}

void TestScene::End()
{
}
