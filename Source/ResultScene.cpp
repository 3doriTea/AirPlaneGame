#include "ResultScene.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"
#include "Draw.h"
#include "ResultLogo.h"
#include <iostream>

#include "../Source/TestScene/PlayerPlane.h"
#include "../Source/TestScene/PlayerGunner.h"
#include "../Source/TestScene/PlayerPilot.h"

ResultScene::ResultScene()
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Initialize()
{
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);

	Audio::Clear();

	Instantiate<SkySphere>();
//	Instantiate<ResultText>();
	Instantiate<TextBox>("tekita", 0.1f);
	Instantiate<ResultLogo>();


}

void ResultScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}
}

void ResultScene::Draw() const
{
	Draw::ImmediateText("Ç†Ç»ÇΩÇÃÉXÉRÉA", { 0, 100 }, 48, TextAlignment::center);
}

void ResultScene::End()
{
}
