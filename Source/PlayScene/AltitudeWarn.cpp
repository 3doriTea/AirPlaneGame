#include "AltitudeWarn.h"

namespace
{
	const RectF DRAW_RECT{ 0, 0, 1920, 1080 };
	const UIParams UI_PARAMS{ .depth = 100, .layerFlag = GameObjectLayer::A };
}

AltitudeWarn::AltitudeWarn() : GameObject(GameObjectBuilder()
	.SetLayerFlag(GameObjectLayer::A)
	.Build()),
	enabled_{ false }
{
	hImage_ = Image::Load("Image/atl.png");
}

AltitudeWarn::~AltitudeWarn()
{
}

void AltitudeWarn::Update()
{
}

void AltitudeWarn::Draw() const
{
	if (enabled_)
	{
		Draw::Image(hImage_, GenDrawScreenFrom(DRAW_RECT), UI_PARAMS);
	}
}
