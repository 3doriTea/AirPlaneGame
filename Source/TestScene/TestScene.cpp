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

	Instantiate<EnemyPlane>(Vector3{ 0, 0, 0 }, eIdPlayer);
	/*Instantiate<EnemyPlane>(Vector3{ 5, -1, 30 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 0, 5, -10 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 10, 0, 30 }, eIdPlayer);*/

	Instantiate<OrderText>(eIdPlayer, GameObjectLayer::A);

	Instantiate<Radar>(eIdPlayer, GameObjectLayer::A);

	Radar* pGunnerRader{ Instantiate<Radar>(eIdPlayer, GameObjectLayer::B) };
	pGunner->SetRadarUI(pGunnerRader);

	Instantiate<HighlightEnemy>();

	ControlTower* pControlTower{ Instantiate<ControlTower>() };
	pControlTower->SetGunner(pGunner->GetEntityId(), hCamera1);
	pControlTower->SetPilot(pPilot->GetEntityId(), hCamera2);

	/*reader16 = new TerrainReader16();
	reader16->ReadTerrain("terrain16.raw");*/

	/*reader8 = new TerrainReader8();
	reader8->Initialize();*/
	//reader8->ReadTerrain("terrain.raw");
	//reader8->GenerateQuadtreeHeightMap();
	//reader8->GenerateTerrainAABBs(&reader8->aabbs);

	Instantiate<QuotaGauge>();
	

	timeLimit_ = Instantiate<TimeLimit>(60.0f);
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
		Game::System<SceneSystem>().Move<PlayScene>();
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
	//reader8->TestDraw();
	
	
}

void TestScene::End()
{
}
