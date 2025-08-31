#include "TutorialScene.h"
#include "../PlayScene.h"

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_PLAY_SCENE_WAIT_SEC{ 60 };
}

TutorialScene::TutorialScene() :
	hToPlaySceneTimer_{ nullptr }
{
}

TutorialScene::~TutorialScene()
{
}

void TutorialScene::Initialize()
{
	// 一定時間経ったら必ずプレイシーンに遷移
	hToPlaySceneTimer_ = Timer::AddAram(TO_PLAY_SCENE_WAIT_SEC, []()
		{
			Game::System<SceneSystem>().Move<PlayScene>();
		});

	state_
		.OnUpdate(S_HANDSHAKE, []
		{
		});
}

void TutorialScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<PlayScene>();
	}

	state_.Update();
}

void TutorialScene::Draw() const
{
}

void TutorialScene::End()
{
}
