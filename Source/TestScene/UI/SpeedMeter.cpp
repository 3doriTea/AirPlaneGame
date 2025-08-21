#include "SpeedMeter.h"

using namespace mtgb;

SpeedMeter::SpeedMeter() : GameObject(GameObjectBuilder()
	.SetLayerFlag(GameObjectLayer::A)
	.Build()),
	speed_{ 0.0f }
{
}

SpeedMeter::~SpeedMeter()
{
}

void SpeedMeter::Update()
{
}

void SpeedMeter::Draw() const
{
	//Draw::ImmediateText()
}
