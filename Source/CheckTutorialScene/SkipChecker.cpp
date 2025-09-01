#include "SkipChecker.h"
#include "CheckTutorialScene.h"

namespace
{
	const float MOVE_RATE_SEC{ 0.1f * 20.0 };
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
		rate_ = Mathf::Lerp(rate_, 1.0f, MOVE_RATE_SEC * Time::DeltaTimeF());
	}
	else
	{
		rate_ = Mathf::Lerp(rate_, 0.0f, MOVE_RATE_SEC * Time::DeltaTimeF());
	}

	GetScene<CheckTutorialScene>().SetPeekRate(rate_);
}

void SkipChecker::Draw()
{
}
