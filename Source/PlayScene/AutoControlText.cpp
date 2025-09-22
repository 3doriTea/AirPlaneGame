#include "AutoControlText.h"

using namespace mtgb;

namespace
{
	const RectF DRAW_RECT{ 0, 0, 1920, 1080 };
	const UIParams UI_PARAMS{ .depth = 100, .layerFlag = GameObjectLayer::A, };
}

AutoControlText::AutoControlText() : GameObject(GameObjectBuilder()
	.SetLayerFlag(GameObjectLayer::A)
	.Build()),
	enabled_{ false }
{
	hImage_ = Image::Load("Image/AutoControlling.png");
}

AutoControlText::~AutoControlText()
{
}

void AutoControlText::Update()
{
}

void AutoControlText::Draw() const
{
	if (enabled_)
	{
		Draw::Image(hImage_, GenDrawScreenFrom(DRAW_RECT), UI_PARAMS);
	}
}
