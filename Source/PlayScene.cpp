#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"

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
	SetCameraGameObject(Instantiate<Camera>());

	Instantiate<Player>(WindowContext::First);
	//Instantiate<Player>(WindowContext::Second);
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
