#include "ImageBlinker.h"


namespace
{
	const float DEFAULT_INTERVAL_SEC{ 0.5f };
}
ImageBlinker::ImageBlinker(ImageHandle _image, const RectF& _drawRect)
	: image_{ _image }
	, drawRect_{ _drawRect }
	, visible_{false}
	, hInterval_{nullptr}
	, hStop_{nullptr}
	, currentIntervalTimeSec_{0.5f}
{
}

ImageBlinker::~ImageBlinker()
{
}

void ImageBlinker::Draw() const
{
	if (!visible_) return;

	Draw::Image(image_, GenDrawScreenFrom(drawRect_));
}

void ImageBlinker::StartBlink(float _intervalSec)
{
	currentIntervalTimeSec_ = (_intervalSec > 0.0f) ? _intervalSec : DEFAULT_INTERVAL_SEC;
	visible_ = true;

	hInterval_ = Game::System<Timer>().AddInterval(currentIntervalTimeSec_,
		[this]()
		{
			visible_ = !visible_;
		},/*firstCall=*/true);
}

void ImageBlinker::StartBlinkWithTimeout(float _intervalSec, float _durationSec)
{
	StartBlink(_intervalSec);
	
	if (_durationSec <= 0.0f)
	{
		return;
	}

	hStop_ = Game::System<Timer>().AddAram(_durationSec,
		[this]()
		{
			StopBlink();
		});
}

void ImageBlinker::StopBlink()
{
	if (hInterval_)
	{
		Game::System<Timer>().Remove(hInterval_);
		hInterval_ = nullptr;
	}
	visible_ = false;
}
