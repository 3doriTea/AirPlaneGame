#include "TestScene.h"
#include "../PlayScene.h"
#include "../Reticle.h"
#include "../SkySphere.h"
#include "../Enemy.h"

#include "PlayerPlane.h"
#include "PlayerGunner.h"
#include "PlayerPilot.h"
#include "EnemyPlane.h"

using namespace mtgb;

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
	Audio::Clear();

	Instantiate<SkySphere>();
	Instantiate<Reticle>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>() };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };

	CameraHandleInScene hCamera1 = RegisterCameraGameObject(
		Instantiate<PlayerPilot>(eIdPlayer));

	CameraHandleInScene hCamera2 = RegisterCameraGameObject(
		Instantiate<PlayerGunner>(eIdPlayer));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	//Instantiate<EnemyPlane>(Vector3{ 0, 10, 10 }, eIdPlayer);
	//Instantiate<EnemyPlane>(Vector3{ 0, 20, 20 }, eIdPlayer);
	//Instantiate<EnemyPlane>(Vector3{ 0, 10, 0 }, eIdPlayer);
	Instantiate<EnemyPlane>(Vector3{ 10, 0, 30 }, eIdPlayer);
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
