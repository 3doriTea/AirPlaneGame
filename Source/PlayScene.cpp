#include "PlayScene.h"
#include "Player.h"
#include "Camera.h"

using namespace mtgb;

PlayScene::PlayScene()
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	SetCameraGameObject(Instantiate<Camera>());

	Instantiate<Player>();
}

void PlayScene::Update()
{
	if (InputData::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}
	//LOGF("ƒeƒXƒg");
}

void PlayScene::Draw() const
{
}

void PlayScene::End()
{
}
