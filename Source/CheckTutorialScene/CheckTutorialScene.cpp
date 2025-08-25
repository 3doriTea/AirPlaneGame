#include "CheckTutorialScene.h"
#include "../TutorialScene/TutorialScene.h"
#include "../PlayScene.h"

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_NEXT_SCENE_WAIT_SEC{ 5.0f };
}

CheckTutorialScene::CheckTutorialScene()
{
}

CheckTutorialScene::~CheckTutorialScene()
{
}

void CheckTutorialScene::Initialize()
{
	// 一定時間経ったら必ずプレイシーンに遷移
	hToNextSceneTimer_ = Timer::AddAram(TO_NEXT_SCENE_WAIT_SEC, []()
		{
			Game::System<SceneSystem>().Move<PlayScene>();
		});
}

void CheckTutorialScene::Update()
{
	isSkipTutorial_ = InputUtil::GetAxis(Axis::Y, WindowContext::Second) < 0;
}

void CheckTutorialScene::Draw() const
{
}

void CheckTutorialScene::End()
{
}
