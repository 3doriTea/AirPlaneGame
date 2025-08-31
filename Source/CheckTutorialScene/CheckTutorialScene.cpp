#include "CheckTutorialScene.h"
#include "../TutorialScene/TutorialScene.h"
#include "../PlayScene.h"
#include "SkipChecker.h"

using namespace mtgb;

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_NEXT_SCENE_WAIT_SEC{ 10.0f };
	const float TO_MOVE_SCENE_WAIT_SEC{ 11.0f };
	const Vector2Int BACKGROUND_IMAGE_SIZE{ 1920, 1080 };
	const Vector2 CANVAS_SIZE{ 1920.0f, 1080.0f };
	const float TO_SKIP_THRESHOLD{ 0.7 };  // チュートリアルをスキップするしきい値
}

CheckTutorialScene::CheckTutorialScene() :
	peekRate_{ 0.0f },
	toMoveTimeLeft_{ TO_MOVE_SCENE_WAIT_SEC }
{
}

CheckTutorialScene::~CheckTutorialScene()
{
}

void CheckTutorialScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	hImage_[IL_BACKGROUND] = Image::Load("Image/CheckTutorialBackground.png");
	hImage_[IL_MOUNTAIN] = Image::Load("Image/CheckTutorialBackgroundMt.png");
	hImage_[IL_PLANE] = Image::Load("Image/CheckTutorialBackgroundPl.png");
	hImage_[IL_MESSAGE] = Image::Load("Image/CheckTutorialText1.png");

	// 一定時間経ったら必ず次のシーンに遷移
	hToNextSceneTimer_ = Timer::AddAram(TO_NEXT_SCENE_WAIT_SEC, [this]()
		{
			if (peekRate_ >= TO_SKIP_THRESHOLD)
			{
				Game::System<SceneSystem>().Move<PlayScene>();
			}
			else
			{
				Game::System<SceneSystem>().Move<TutorialScene>();
			}
		});

	Instantiate<SkipChecker>();
}

void CheckTutorialScene::Update()
{
	toMoveTimeLeft_ -= Time::DeltaTimeF();

	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}
}

void CheckTutorialScene::Draw() const
{
	static auto drawImageLayer
	{
		[this](const IMAGE_LAYER _layer, const Vector2Int _position)
		{
			Draw::Image(hImage_[_layer], { _position, Game::System<Screen>().GetSize() }, { _layer });
		}
	};
	static auto drawImageLayerRect
	{
		[this](const IMAGE_LAYER _layer, const RectF _rect)
		{
			Draw::Image(hImage_[_layer], _rect, { _layer });
		}
	};

	drawImageLayer(IL_BACKGROUND, Vector2Int::Zero());
	drawImageLayer(IL_PLANE, GenPositionPlane());
	drawImageLayer(IL_MOUNTAIN, GenPositionMountain());
	drawImageLayerRect(IL_MESSAGE, GenTextBoxMsg());

	std::string text{ std::string{ std::to_string(toMoveTimeLeft_).substr(0, 3) } + "秒" };

	Draw::ImmediateText(text, GenTextBoxTimer(), 82, TextAlignment::middleLeft, { 0 });
	
	RectF box{ GenSlideBar() };
	Draw::Box(box.GetBegin(), box.GetEnd(), Color::WHITE, { 0 });

	RectF solid{ GenSlideBar(toMoveTimeLeft_ / TO_MOVE_SCENE_WAIT_SEC) };
	Draw::Box(solid.GetBegin(), solid.GetEnd(), Color::GREEN, { 1 });

	//Draw::Text()
}

void CheckTutorialScene::End()
{
}

const Vector2Int CheckTutorialScene::GenPositionPlane() const
{
	static const Vector2Int TO{ -100, -300 };
	static const Vector2 TO_RATE{ TO.x / CANVAS_SIZE.x, TO.y / CANVAS_SIZE.y };
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	return Vector2Int{ SCREEN_SIZE.x * TO_RATE.x, SCREEN_SIZE.y * TO_RATE.y } * peekRate_;
}

const Vector2Int CheckTutorialScene::GenPositionMountain() const
{
	static const int TO_Y{ 500 };
	static const float TO_Y_RATE{ TO_Y / CANVAS_SIZE.y };

	return Vector2Int(0, Game::System<Screen>().GetSize().y * TO_Y_RATE * peekRate_);
}

const RectF CheckTutorialScene::GenTextBoxTimer() const
{
	static const RectF TO{ 900, 375, 700, 90 };
	static const RectF RATE
	{
		TO.point.x / CANVAS_SIZE.x, TO.point.y / CANVAS_SIZE.y,
		TO.size.x / CANVAS_SIZE.x, TO.size.y / CANVAS_SIZE.y,
	};
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	return
	{
		RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
		RATE.size.x * SCREEN_SIZE.x, RATE.size.y * SCREEN_SIZE.y,
	};
}

const RectF CheckTutorialScene::GenTextBoxMsg() const
{
	static const RectF TO{ 490, 385, 430, 80 };
	static const RectF RATE
	{
		TO.point.x / CANVAS_SIZE.x, TO.point.y / CANVAS_SIZE.y,
		TO.size.x / CANVAS_SIZE.x, TO.size.y / CANVAS_SIZE.y,
	};
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	return
	{
		RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
		RATE.size.x * SCREEN_SIZE.x, RATE.size.y * SCREEN_SIZE.y,
	};
}

const RectF CheckTutorialScene::GenSlideBar(const float _rate) const
{
	static const RectF TO{ 240, 480, 1440, 40 };
	static const RectF RATE
	{
		TO.point.x / CANVAS_SIZE.x, TO.point.y / CANVAS_SIZE.y,
		TO.size.x / CANVAS_SIZE.x, TO.size.y / CANVAS_SIZE.y,
	};
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	return
	{
		RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
		Mathf::Lerp(0.0f, RATE.size.x * SCREEN_SIZE.x, _rate), RATE.size.y * SCREEN_SIZE.y,
	};
}
