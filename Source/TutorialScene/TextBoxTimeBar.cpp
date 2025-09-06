#include "TextBoxTimeBar.h"
#include "DrawScreenUtility.h"

namespace
{
	const RectF BEGIN_RECT{ 280, 900, 1360, 20 };
	const float END_WIDTH{ 0.0f };
	const float END_X{ 960.0f };
	const float RATE_MAX{ 1.0f };

	const RectF DRAW_RECT_FRAME{ 0, 0, 1000, 700 };
}

TextBoxTimeBar::TextBoxTimeBar() : GameObject(GameObjectBuilder()
	.Build())
{
}

TextBoxTimeBar::~TextBoxTimeBar()
{
}

void TextBoxTimeBar::Update()
{
	if (timeLeft_ >= 0.0f)
	{
		timeLeft_ -= Time::DeltaTimeF();
	}
}

void TextBoxTimeBar::Draw() const
{
	if (timeLeft_ < 0.0f)
	{
		return;  // 残り時間がマイナスなら回帰
	}

	float rate{ RATE_MAX - timeLeft_ / timeLeftMax_ };
	RectF draw{ BEGIN_RECT };

	draw.x = Mathf::Lerp(BEGIN_RECT.x, END_X, rate);
	draw.width = Mathf::Lerp(END_WIDTH, BEGIN_RECT.width, rate);

	Draw::Box(GenDrawScreenFrom(RectInt{ draw.x, draw.y, draw.width, draw.height }), Color::GREEN, { 0 });
}

void TextBoxTimeBar::SetTimeLeftMax(const float _timeSec)
{
	timeLeftMax_ = _timeSec;
	timeLeft_ = timeLeftMax_;
}
