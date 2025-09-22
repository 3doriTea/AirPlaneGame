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

	// 一つ目のウィンドウ
	RectDetectorConfig firstConfig =
	{
		.base = 
		{
			.targetTag = GameObjectTag::Enemy,
			.windowContext = WindowContext::First,
			.maxDistance = distance,
			.minDistance = 0.0f,
		},
		.detectionRect =
		{
			0.0f,
			0.0f,
			static_cast<float>(firstWndRect.right),
			static_cast<float>(firstWndRect.bottom),
		}
	};
	firstWndRectDetector_.config = firstConfig;
	// 二つ目のウィンドウ
	RectDetectorConfig secondConfig = firstConfig;
	
	secondConfig.detectionRect.width = static_cast<float>(secondWndRect.right);
	secondConfig.detectionRect.height = static_cast<float>(secondWndRect.bottom);
	secondConfig.base.windowContext = WindowContext::Second;

	secondWndRectDetector_.config = secondConfig;
}

HighlightEnemy::~HighlightEnemy()
{
}

void HighlightEnemy::Update()
{
	firstWndRectDetector_.UpdateDetection();
	secondWndRectDetector_.UpdateDetection();

	/*MTImGui::Instance().DirectShow([this]() {
		for (const auto& target : secondWndRectDetector_.detectedTargets)
		{
			ImGui::Text("%lld", target.entityId);
		}
		}, "Contains", ShowType::Inspector);*/
	/*for (const auto& target : secondWndRectDetector_.detectedTargets)
	{
		MTImGui::Instance().TypedShow(&secondWndRectDetector_.detectedTargets, "SecondWndRectDetector", ShowType::Inspector);
	}*/
	MTImGui::Instance().TypedShow(&secondWndRectDetector_, "SecondWndRectDetector", ShowType::Inspector);
}

void HighlightEnemy::Draw() const
{
	// 一つ目のウィンドウ
	const RectDetectorConfig& firstConfig = firstWndRectDetector_.config;

	Vector2F ratio = Game::System<Screen>().GetSizeRatio();

	firstWndRectDetector_.ForEach([this,ratio](const ScreenCoordContainsInfo& _info)
		{
			if (_info.screenPos.z > 0.0f && _info.screenPos.z < 1.0f)
			{
				RectF targetRect
				{
					(_info.screenPos.x - highlightFrameSize.x * ratio.x / 2.0f),
					(_info.screenPos.y - highlightFrameSize.y * ratio.y / 2.0f),
					highlightFrameSize.x * ratio.x,
					highlightFrameSize.y * ratio.y
				};
				Draw::Image(highlightFrameImage_, targetRect, { .depth = 0,.layerFlag = GameObjectLayer::A });
			}
		});

	// 二つ目のウィンドウ
	const RectDetectorConfig& secondConfig = secondWndRectDetector_.config;

	secondWndRectDetector_.ForEach([this,ratio](const ScreenCoordContainsInfo& _info)
		{
			if (_info.screenPos.z > 0.0f && _info.screenPos.z < 1.0f)
			{
				RectF targetRect
				{
					(_info.screenPos.x - highlightFrameSize.x * ratio.x / 2.0f),
					(_info.screenPos.y - highlightFrameSize.y * ratio.y / 2.0f),
					highlightFrameSize.x * ratio.x,
					highlightFrameSize.y * ratio.y
				};
				Draw::Image(highlightFrameImage_, targetRect, { .depth = 0,.layerFlag = GameObjectLayer::B });
			}
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