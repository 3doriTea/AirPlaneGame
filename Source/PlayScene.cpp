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

	// �\���������e�L�X�g���J�n
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");
}

void PlayScene::Update()
{
	if (InputData::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}
	//LOGF("�e�X�g");
}

void PlayScene::Draw() const
{
	tBox_->Draw();
}

void PlayScene::End()
{
}
