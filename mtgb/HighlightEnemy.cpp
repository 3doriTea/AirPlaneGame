#include "HighlightEnemy.h"

namespace
{
	Vector2F highlightFrameSize;

}

HighlightEnemy::HighlightEnemy() : GameObject(GameObjectBuilder()
	.Build())
{
	RECT firstWndRect;
	GetWindowRect(WinCtxRes::GetHWND(WindowContext::First), &firstWndRect);
	RECT secondWndRect;
	GetWindowRect(WinCtxRes::GetHWND(WindowContext::Second), &secondWndRect);

	float distance = Game::System<CameraSystem>().GetFar();
	highlightFrameImage_ = Image::Load("Image/highlightEnemyFrame.png");
	highlightFrameSize = {60.0f,60.0f};

	firstWndRectDetector_.config  =
	{
		.targetName = "Enemy",
		.windowContext = WindowContext::First,
		.detectionRect =
		{
			0.0f,
			0.0f,
			static_cast<float>(firstWndRect.right),
			static_cast<float>(firstWndRect.bottom),
		},
		.maxDistance = distance,
	};

	secondWndRectDetector_.config =
	{
		.targetName = "Enemy",
		.windowContext = WindowContext::Second,
		.detectionRect =
		{
			0.0f,
			0.0f,
			static_cast<float>(secondWndRect.right),
			static_cast<float>(secondWndRect.bottom),
		},
		.maxDistance = distance,
	};
}

HighlightEnemy::~HighlightEnemy()
{
}

void HighlightEnemy::Update()
{
	firstWndRectDetector_.UpdateDetection();
	secondWndRectDetector_.UpdateDetection();
}

void HighlightEnemy::Draw() const
{
	// 一つ目のウィンドウ
	const RectDetectorConfig& firstConfig = firstWndRectDetector_.config;

	firstWndRectDetector_.ForEach([this](const RectContainsInfo& _info)
		{
			RectF targetRect
			{
				_info.screenPos.x - highlightFrameSize.x / 2.0f,
				_info.screenPos.y - highlightFrameSize.y / 2.0f,
				highlightFrameSize.x,
				highlightFrameSize.y
			};
			Draw::Image(highlightFrameImage_, targetRect, { .depth = 0,.layerFlag = GameObjectLayer::A });
		});

	// 二つ目のウィンドウ
	const RectDetectorConfig& secondConfig = secondWndRectDetector_.config;

	firstWndRectDetector_.ForEach([this](const RectContainsInfo& _info)
		{
			RectF targetRect
			{
				_info.screenPos.x - highlightFrameSize.x / 2.0f,
				_info.screenPos.y - highlightFrameSize.y / 2.0f,
				highlightFrameSize.x,
				highlightFrameSize.y
			};
			Draw::Image(highlightFrameImage_, targetRect, { .depth = 0,.layerFlag = GameObjectLayer::B });
		});
}

void HighlightEnemy::Highlight()
{
	
}

//void mtgb::RectDetector::DrawTargetImage() const
//{
//	if (detectedTargets.empty())
//	{
//		return;
//	}
//
//	for (const auto& target : detectedTargets)
//	{
//		RectF targetRect
//		{
//			target.screenPos.x - config.drawPosOffset,
//			target.screenPos.y - config.drawPosOffset,
//			config.drawPosOffset * 2,
//			config.drawPosOffset * 2
//		};
//		Draw::Image(config.displayImage, targetRect, config.uiParams);
//	}
//}