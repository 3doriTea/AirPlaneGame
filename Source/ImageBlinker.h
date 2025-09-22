#pragma once
#include <mtgb.h>

class ImageBlinker
{
public:
	ImageBlinker(ImageHandle _image, const RectF & _drawRect);
	~ImageBlinker();

	/// <summary>
	/// 点滅開始、明示的に停止をする必要がある
	/// </summary>
	/// <param name="_intervalSec"></param>
	void StartBlink(float _intervalSec);

	/// <summary>
	/// 点滅開始、自動で停止する
	/// </summary>
	/// <param name="_intervalSec"></param>
	/// <param name="_durationSec"></param>
	void StartBlinkWithTimeout(float _intervalSec, float _durationSec);

	/// <summary>
	/// 点滅を停止させる
	/// </summary>
	void StopBlink();

	//void ForceStop();

	void Draw() const;

private:
	ImageHandle image_;
	RectF drawRect_;
	Vector2F size_;
	bool visible_;

	TimerHandle hInterval_;
	TimerHandle hStop_;
	float currentIntervalTimeSec_;
};