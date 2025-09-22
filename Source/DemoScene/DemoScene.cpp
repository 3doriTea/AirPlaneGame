#include "DemoScene.h"
#include "../CheckTutorialScene/CheckTutorialScene.h"
#include "../TestScene/TestScene.h"

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
		Game::System<SceneSystem>().Move<TestScene>();
	}
}

void DemoScene::Draw() const
{
}

void DemoScene::End()
{
}
