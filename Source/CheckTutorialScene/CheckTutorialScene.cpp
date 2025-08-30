#include "CheckTutorialScene.h"
#include "../TutorialScene/TutorialScene.h"
#include "../PlayScene.h"

using namespace mtgb;

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_NEXT_SCENE_WAIT_SEC{ 5.0f };
	const Vector2Int BACKGROUND_IMAGE_SIZE{ 1920, 1080 };
}

CheckTutorialScene::CheckTutorialScene()
{
}

CheckTutorialScene::~CheckTutorialScene()
{
}

void CheckTutorialScene::Initialize()
{
	hBackground_ = Image::Load("Image/CheckTutorialBackground.png");

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
	Draw::Image(hBackground_, { { 0, 0 }, BACKGROUND_IMAGE_SIZE });
}

void CheckTutorialScene::End()
{
}
