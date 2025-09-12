#include "OverScene.h"

#include "ResultScene.h"
#include "TimeLimit.h"

namespace
{
	bool quotaAchieved = false; //ÉmÉãÉ}íBê¨ÇµÇΩÇ©(âº)
	TimeLimit* timeLimit_;
}
OverScene::OverScene()
{
}

OverScene::~OverScene()
{
}

void OverScene::Initialize()
{
	quotaAchieved = Game::System<ScoreManager>().AchievedQuota();

	timeLimit_ = Instantiate<TimeLimit>(10.0f);
	timeLimit_->StartTimer();
	timeLimit_->RegisterOnEndTimerCallback([]()
		{
			Game::System<SceneSystem>().Move<ResultScene>();

		});
}

void OverScene::Update()
{
	/*if (InputUtil::GetKey(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<ResultScene>();
	}*/
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
