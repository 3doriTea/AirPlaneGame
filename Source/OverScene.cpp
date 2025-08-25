#include "OverScene.h"

#include "EndScene.h"
OverScene::OverScene()
{
}

OverScene::~OverScene()
{
}

void OverScene::Initialize()
{

}

void OverScene::Update()
{
	if (InputUtil::GetKey(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<EndScene>();
	}
}

void OverScene::Draw() const
{
	Draw::ImmediateText("Over!!", { 0,0 }, 100, TextAlignment::center);
}

void OverScene::End()
{
}
