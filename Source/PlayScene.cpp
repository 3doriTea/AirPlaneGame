#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "CameraSystem.h"
using namespace mtgb;

namespace
{
	
}

PlayScene::PlayScene()
{
	tBox_ = new TextBox();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	//SetCameraGameObject(Instantiate<Camera>());
	//WinCtxRes::Get<CameraResource>(WindowContext::First).SetCamera(Instantiate<Camera>(WindowContext::First));
	//WinCtxRes::Get<CameraResource>(WindowContext::Second).SetCamera(Instantiate<Camera>(WindowContext::Second));
	//hCamera1_ = Game::System<CameraSystem>().RegisterCamera()
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);

	Instantiate<Player>(WindowContext::First);
	Instantiate<Player>(WindowContext::Second);
	Instantiate<Enemy>();

	// 表示したいテキストを開始
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");
}

void PlayScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::Escape,mtgb::WindowContext::Both))
	{
		Game::Exit();
	}
	//LOGF("テスト");
}

void PlayScene::Draw() const
{
	tBox_->Draw();
}

void PlayScene::End()
{
}
