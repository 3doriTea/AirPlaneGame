#include "OverScene.h"

#include "EndScene.h"

namespace
{
	constexpr int score = 300; // スコア(仮)
	constexpr int quota = 100; // ノルマ(仮)
	bool quotaAchieved = false; //ノルマ達成したか(仮)
}
OverScene::OverScene()
{
}

OverScene::~OverScene()
{
}

void OverScene::Initialize()
{
	if (score >= quota)
	{
		quotaAchieved = true;
	}
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
	if(quotaAchieved)
	{
		Draw::ImmediateText("Clear!!", { 0,0 }, 100, TextAlignment::center);
	}
	else
	{
		Draw::ImmediateText("GameOver!!", { 0,0 }, 100, TextAlignment::center);

	}
}

void OverScene::End()
{
}
