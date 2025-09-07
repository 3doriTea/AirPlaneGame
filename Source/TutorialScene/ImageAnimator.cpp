#include "ImageAnimator.h"
#include "DrawScreenUtility.h"

using namespace mtgb;

ImageAnimator::ImageAnimator(
	const Setting& _setting,
	const GameObjectLayer _layer) : GameObject(GameObjectBuilder()
		.SetLayerFlag(_layer)
	.Build()),
	showIndex_{ 0 },
	timeLeft_{ 0.0f },
	setting_{ _setting }
{
}

ImageAnimator::~ImageAnimator()
{
}

void ImageAnimator::Update()
{
	timeLeft_ -= Time::DeltaTimeF();
	if (timeLeft_ <= 0.0f)
	{
		showIndex_++;
		showIndex_ %= setting_.elements_.size();

		Element& nextElement{ setting_.elements_[showIndex_] };
		if (nextElement.timeSec_ <= 0.0f)  // 0以下ならデフォルト時間を使う
		{
			timeLeft_ += setting_.defaultTimeSec_;
		}
		else
		{
			timeLeft_ += nextElement.timeSec_;
		}
	}
}

void ImageAnimator::Draw() const
{
	const Element& element{ setting_.elements_[showIndex_] };
	Draw::Image(element.hImage_, GenDrawScreenFrom(setting_.drawRect_), setting_.uIParams_);
}
