#include "TimeLimit.h"
#include <chrono>
#include <format>
namespace chrono = std::chrono;
using namespace std::chrono_literals;

TimeLimit::TimeLimit() : GameObject(GameObjectBuilder()
	.SetName("TimeLimit")
	.Build())
	,remainingTime_{PLAY_SCENE_TIMER}
	,isStartTimer_{false}
	, isResumeTimer_{false}
{
}

TimeLimit::TimeLimit(float _time) : TimeLimit()
{
	remainingTime_ = _time;
}

TimeLimit::~TimeLimit()
{
}

void TimeLimit::Update()
{
	// タイマーのカウントダウンが始まっていない、一時停止中なら回帰
	if (!isStartTimer_ || isResumeTimer_) return;

	remainingTime_ -= Time::DeltaTimeF();

	if (remainingTime_ <= 0.0f)
	{
		isStartTimer_ = false;

		remainingTime_ = 0.0f;

		// コールバックを一斉呼び出し
		while (!onEndTimerCallbacks_.empty())
		{
			onEndTimerCallbacks_.front()();
			onEndTimerCallbacks_.pop();
		}
	}
}

void TimeLimit::Draw() const
{
	// 一時停止中でも描画を行う
	if (isStartTimer_)
	{
		chrono::seconds sec{ static_cast<int>(remainingTime_) };
		chrono::hh_mm_ss<chrono::seconds> time(sec);
		// 2桁をゼロ埋め		
		Draw::ImmediateText(std::format("{:02}:{:02}", time.minutes().count(),time.seconds().count()), Vector2F{0.0f,0.0f}, 72, TextAlignment::topLeft, UIParams{.depth = 0,.layerFlag = mtbit::operator|(GameObjectLayer::A ,GameObjectLayer::B)});
	}
	
	
}

void TimeLimit::StartTimer()
{
	isStartTimer_ = true;
	isResumeTimer_ = false;
}


void TimeLimit::RegisterOnEndTimerCallback(std::function<void()> _callback)
{
	onEndTimerCallbacks_.push(_callback);
}

void TimeLimit::PauseTimer()
{
	isResumeTimer_ = true;
}

void TimeLimit::ResumeTimer()
{
	isResumeTimer_ = false;
}

void TimeLimit::Reset()
{
	isStartTimer_ = false;
	isResumeTimer_ = false;
	remainingTime_ = PLAY_SCENE_TIMER;
	while (!onEndTimerCallbacks_.empty())
	{
		onEndTimerCallbacks_.pop();
	}
}