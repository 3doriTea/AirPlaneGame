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

	CameraHandleInScene hCamera1 = RegisterCameraGameObject(
		Instantiate<PlayerPilot>(eIdPlayer));

	PlayerGunner* pGunner{ Instantiate<PlayerGunner>(eIdPlayer) };
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pGunner);

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	Instantiate<EnemyPlane>(Vector3{ 0, 3, 40 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 5, -1, 30 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 0, 0, -10 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 10, 0, 30 }, eIdPlayer);

	Instantiate<Radar>(eIdPlayer, GameObjectLayer::A);

	Radar* pGunnerRader{ Instantiate<Radar>(eIdPlayer, GameObjectLayer::B) };
	pGunner->SetRadarUI(pGunnerRader);

	Instantiate<HighlightEnemy>();
}

void TestScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<PlayScene>();
	}
}

void TestScene::Draw() const
{
}

void TestScene::End()
{
}
