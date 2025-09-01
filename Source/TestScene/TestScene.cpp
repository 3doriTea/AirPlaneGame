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

using namespace mtgb;

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
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

	Instantiate<EnemyPlane>(Vector3{ 0, 3, 40 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 5, -1, 30 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 0, 5, -10 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 10, 0, 30 }, eIdPlayer);

	Instantiate<OrderText>(eIdPlayer, GameObjectLayer::A);

	Instantiate<Radar>(eIdPlayer, GameObjectLayer::A);

	Radar* pGunnerRader{ Instantiate<Radar>(eIdPlayer, GameObjectLayer::B) };
	pGunner->SetRadarUI(pGunnerRader);

	Instantiate<HighlightEnemy>();

	ControlTower* pControlTower{ Instantiate<ControlTower>() };
	pControlTower->SetGunner(pGunner->GetEntityId(), hCamera1);
	pControlTower->SetPilot(pPilot->GetEntityId(), hCamera2);
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
	
	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}

	if (InputUtil::GetKeyDown(KeyCode::F11))
	{
		static bool flag = false;
		if (flag)
		{
			flag = false;
		}
		else
		{
			flag = true;
		}
		WinCtxRes::SetFullscreen(flag, WindowContext::First);
	}
	if (InputUtil::GetKeyDown(KeyCode::F10))
	{
		static bool flag = false;
		if (flag)
		{
			flag = false;
		}
		else
		{
			flag = true;
		}
		WinCtxRes::SetFullscreen(flag, WindowContext::Second);
	}

}

void TestScene::Draw() const
{
}

void TestScene::End()
{
}
