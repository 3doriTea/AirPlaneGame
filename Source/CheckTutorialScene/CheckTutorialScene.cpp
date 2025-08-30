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
	hImage_[IL_BACKGROUND] = Image::Load("Image/CheckTutorialBackground.png");
	hImage_[IL_MOUNTAIN] = Image::Load("Image/CheckTutorialBackgroundMt.png");
	hImage_[IL_PLANE] = Image::Load("Image/CheckTutorialBackgroundPl.png");

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
	static auto drawImageLayer
	{
		[this](const IMAGE_LAYER _layer, const Vector2Int _position)
		{
			Draw::Image(hImage_[_layer], {_position, BACKGROUND_IMAGE_SIZE}, { _layer });

		}
	};

	drawImageLayer(IL_BACKGROUND, Vector2Int::Zero());
	drawImageLayer(IL_PLANE, Vector2Int::Zero());
	drawImageLayer(IL_MOUNTAIN, Vector2Int::Zero());
}

void CheckTutorialScene::End()
{
}
