#include "DemoScene.h"
#include "../TutorialScene/TutorialScene.h"

using namespace mtgb;

DemoScene::DemoScene()
{
}

DemoScene::~DemoScene()
{
}

void DemoScene::Initialize()
{
}

void DemoScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TutorialScene>();
	}
}

void DemoScene::Draw() const
{
}

void DemoScene::End()
{
}
