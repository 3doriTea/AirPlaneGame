#include "SkipChecker.h"
#include "CheckTutorialScene.h"

namespace
{
	const float MOVE_RATE{ 0.01f };
}

SkipChecker::SkipChecker() : GameObject(GameObjectBuilder()
	.Build()),
	rate_{ 0.0f }
{
}

SkipChecker::~SkipChecker()
{
}

void SkipChecker::Update()
{
	if (InputUtil::GetAxis(Axis::Y, WindowContext::Both) < 0 && InputUtil::GetAxis(Axis::Y, WindowContext::Both) < 0)
	{
		rate_ = Mathf::Lerp(rate_, 1.0f, MOVE_RATE);
	}
	else
	{
		rate_ = Mathf::Lerp(rate_, 0.0f, MOVE_RATE);
	}

	GetScene<CheckTutorialScene>().SetPeekRate(rate_);
}

void SkipChecker::Draw()
{
}
