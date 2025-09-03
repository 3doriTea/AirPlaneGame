#include "TextBoxBackground.h"

using namespace mtgb;

namespace
{
	//const RectF DRAW_RECT{ 0, 0, 1000, 700 };
	const RectF DRAW_RECT{ 278, 778, 1363, 144 };
}

TextBoxBackground::TextBoxBackground() : GameObject(GameObjectBuilder()
	//.SetLayerFlag(GameObjectLayer::A)
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
	//Draw::Image(hImage_, DRAW_RECT, { 5 });
	Draw::Image(hImage_, GenDrawScreenFrom(DRAW_RECT), { 5 });
}
