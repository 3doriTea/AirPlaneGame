#include "PlayScene.h"
#include "Player.h"
#include "Camera.h"

using namespace mtgb;

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

	Instantiate<Player>();

	// 表示したいテキストを開始
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");
}

void PlayScene::Update()
{
	if (InputData::GetKeyDown(KeyCode::Escape))
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
