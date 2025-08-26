#include "EndScene.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"
#include "Draw.h"
#include "EndText.h"
#include "EndLogo.h"

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
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);

	Audio::Clear();

	Instantiate<SkySphere>();
//	Instantiate<EndText>();
	Instantiate<TextBox>("tekita", 0.1f);
	Instantiate<EndLogo>();
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
