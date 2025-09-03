#include "TextBoxTimeBar.h"

namespace
{
	const RectInt DRAW_RECT{ 280, 900, 1360, 20 };
	const RectF DRAW_RECT_FRAME{ 278, 779, 1363, 144 };
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
}

void TextBoxTimeBar::Draw() const
{
	if (timeLeft_ < 0.0f)
	{
		return;  // 残り時間がマイナスなら回帰
	}

	// TODO: 時間経過と共に両端から消えていくように描画する
	Draw::Box(DRAW_RECT, Color::GREEN, { 0 });
}

void TextBoxTimeBar::SetTimeLeftMax(const float _timeSec)
{
	timeLeftMax_ = _timeSec;
	timeLeft_ = timeLeftMax_;
}
