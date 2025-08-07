#include "TestScene.h"
#include "../PlayScene.h"

using namespace mtgb;

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
	Audio::Clear();
}

void TestScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<PlayScene>();
	}
}

void TestScene::Draw() const
{
}

void TestScene::End()
{
}
