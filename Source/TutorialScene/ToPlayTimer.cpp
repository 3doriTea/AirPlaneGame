#include "ToPlayTimer.h"
#include <format>

using namespace mtgb;

namespace
{
	char BACK_IMAGE_FILE[]{ "Image/TutorialTimerBack.png" };
	const float BEGIN_POS_Y{ -202.0f };  // 開始地点のy座標
	// 表示中のパネル範囲
	const RectF DRAW_RECT_STAY_PANEL{ 118.0f, 0.0f, 244.0f, 202.0f };
	// 表示中の文字範囲
	const RectF DRAW_RECT_STAY_TEXT{ 200.0f, 140.0f, 150.0f, 40.0f };
	const UIParams UI_PARAM_PANEL{ 10 };
	const UIParams UI_PARAM_TEXT{ 11 };
	const int TEXT_FONT_SIZE{ 36 };
	const float ANIM_TIME[ToPlayTimer::S_MAX]
	{
		5.0f,
		52.0f,
		3.0f,
	};
}

ToPlayTimer::ToPlayTimer(const float _timeSec) : GameObject(GameObjectBuilder()
	.Build()),
	timeLeft_{ ANIM_TIME[S_ENTER] },
	totalTimeLeft_{ _timeSec }
{
	hBackImage_ = Image::Load(BACK_IMAGE_FILE);

	// 各状態での描画
	stat_
		.OnUpdate(S_ENTER, [this]
			{
				float rate{ 1.0f - timeLeft_ / ANIM_TIME[S_ENTER] };
				RectF draw{ DRAW_RECT_STAY_PANEL };
				draw.y = Mathf::Lerp(BEGIN_POS_Y, DRAW_RECT_STAY_PANEL.y, rate);

				Draw::Image(hBackImage_, GenDrawScreenFrom(draw), UI_PARAM_PANEL);
			})
		.OnUpdate(S_STAY, [this]
			{
				Draw::Image(hBackImage_, GenDrawScreenFrom(DRAW_RECT_STAY_PANEL), UI_PARAM_PANEL);

				Draw::ImmediateText(
					std::format("{:3.1f}", totalTimeLeft_),
					GenDrawScreenFrom(DRAW_RECT_STAY_TEXT),
					TEXT_FONT_SIZE,
					TextAlignment::middleLeft,
					UI_PARAM_TEXT);
			})
		.OnUpdate(S_EXIT, [this]
			{
				float rate{ 1.0f - timeLeft_ / ANIM_TIME[S_EXIT] };
				RectF draw{ DRAW_RECT_STAY_PANEL };
				draw.y = Mathf::Lerp(DRAW_RECT_STAY_PANEL.y, BEGIN_POS_Y, rate);

				Draw::Image(hBackImage_, GenDrawScreenFrom(draw), UI_PARAM_PANEL);
			});
}

ToPlayTimer::~ToPlayTimer()
{
}

void ToPlayTimer::Update()
{
	timeLeft_ -= Time::DeltaTimeF();
	totalTimeLeft_ -= Time::DeltaTimeF();
	if (stat_.Current() == S_EXIT)
	{
		return;
	}

	if (timeLeft_ <= 0.0f)
	{
		STATE current{ stat_.Current() };
		stat_.Change(static_cast<STATE>(current + 1));
		timeLeft_ += ANIM_TIME[stat_.Current()];
	}
}

void ToPlayTimer::Draw() const
{
	stat_.Update();
}
