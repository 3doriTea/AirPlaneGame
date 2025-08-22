#include "EndScene.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"

#include "../Source/TestScene/PlayerPlane.h"
#include "../Source/TestScene/PlayerGunner.h"
#include "../Source/TestScene/PlayerPilot.h"

EndScene::EndScene()
{
}

EndScene::~EndScene()
{
}

void EndScene::Initialize()
{
	MTImGui::Instance().Initialize();
	Audio::Clear();

	Instantiate<SkySphere>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>() };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };

	CameraHandleInScene hCamera1 = RegisterCameraGameObject(
		Instantiate<PlayerPilot>(eIdPlayer));

	CameraHandleInScene hCamera2 = RegisterCameraGameObject(
		Instantiate<PlayerGunner>(eIdPlayer));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

}

void EndScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}
}

void EndScene::Draw() const
{
}

void EndScene::End()
{
}
