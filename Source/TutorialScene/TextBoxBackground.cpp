#include "TextBoxBackground.h"

using namespace mtgb;

namespace
{
	const RectF DRAW_RECT{ 278, 778, 1364, 144 };
}

TextBoxBackground::TextBoxBackground() : GameObject(GameObjectBuilder()
	.Build())
{
	hImage_ = Image::Load("Image/TutorialUITextBox.png");
}

TextBoxBackground::~TextBoxBackground()
{
}

void TextBoxBackground::Update()
{
}

void TextBoxBackground::Draw() const
{
	Draw::Image(hImage_, DRAW_RECT, { 5 });
}
