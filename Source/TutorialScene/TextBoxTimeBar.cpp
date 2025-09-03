#include "TextBoxTimeBar.h"
#include "DrawScreenUtility.h"

namespace
{
	const RectInt DRAW_RECT{ 280, 900, 1360, 20 };
	//const RectInt DRAW_RECT{ 0, 0, 1000, 700 };

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
}

void TextBoxTimeBar::Draw() const
{
	if (timeLeft_ < 0.0f)
	{
		return;  // 残り時間がマイナスなら回帰
	}

	// TODO: 時間経過と共に両端から消えていくように描画する
	Draw::Box(GenDrawScreenFrom(DRAW_RECT), Color::GREEN, { 0 });
}

void TextBoxTimeBar::SetTimeLeftMax(const float _timeSec)
{
	timeLeftMax_ = _timeSec;
	timeLeft_ = timeLeftMax_;
}
