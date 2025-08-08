#include "TestScene.h"
#include "../PlayScene.h"
#include "../Reticle.h"
#include "../SkySphere.h"
#include "../Enemy.h"

#include "PlayerPlane.h"
#include "PlayerGunner.h"
#include "PlayerPilot.h"

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

	RegisterCameraGameObject(
		Instantiate<PlayerPilot>(
			pPlayerPlane->GetEntityId()));

	RegisterCameraGameObject(
		Instantiate<PlayerGunner>(
			pPlayerPlane->GetEntityId()));

	Instantiate<Enemy>(Vector3{ 0, 0, 10 });
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
