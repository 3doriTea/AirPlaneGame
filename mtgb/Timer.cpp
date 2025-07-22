#include "Timer.h"
#include "GameTime.h"


TimerHandle mtgb::Timer::AddAram(const float _time, const std::function<void()>& _callback)
{
	QUEUE_ELEMENT* pElement{ new QUEUE_ELEMENT{ _callback, _time } };
	Instance().EnqueueTimer(pElement);
	return reinterpret_cast<TimerHandle>(pElement);
}

TimerHandle mtgb::Timer::AddInterval(const float _time, const std::function<void()>& _callback, const bool _firstCall)
{
	QUEUE_ELEMENT* pElement{ new QUEUE_ELEMENT{ _callback, _firstCall ? 0 : _time } };
	Instance().pReenqueueElements_.insert({ pElement, _time });
	Instance().EnqueueTimer(pElement);
	return reinterpret_cast<TimerHandle>(pElement);
}

void mtgb::Timer::Remove(TimerHandle _hTimer)
{
	QUEUE_ELEMENT* pElement{ reinterpret_cast<QUEUE_ELEMENT*>(_hTimer) };
	if (Instance().pReenqueueElements_.count(pElement) > 0)
	{
		Instance().pReenqueueElements_.erase(pElement);
	}
	for (auto itr = Instance().pTimerQueue_.begin(); itr != Instance().pTimerQueue_.end();)
	{
		if ((*itr) == pElement)
		{
			itr = Instance().pTimerQueue_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void mtgb::Timer::Clear()
{
	for (auto& pTimer : Instance().pTimerQueue_)
	{
		delete pTimer;
	}

	 Instance().pTimerQueue_.clear();
}

void mtgb::Timer::Release()
{
	Instance().Clear();
	if (pInstance_ != nullptr)
	{
		delete pInstance_;
		pInstance_ = nullptr;
	}
}

void mtgb::Timer::Initialize()
{
}

void mtgb::Timer::Update()
{
	if (pTimerQueue_.size() <= 0)
	{
		return;  // タイマーキューが空なら回帰
	}

	auto itr = pTimerQueue_.begin();
	while (true)
	{
		(*itr)->timeLeft -= Time::DeltaTimeF();

		float diff{ (*itr)->timeLeft };  // 差分
		if (diff > 0)  // 差分が0より大きい = 待機中
		{
			break;  // タイマー終了していないため離脱
		}
		else  // 差分が0以下 = タイマー終了
		{
			(*itr)->on();  // 実行

			// もし使いまわしする要素なら
			if (pReenqueueElements_.count(*itr) > 0)
			{
				QUEUE_ELEMENT* pElement = *itr;
				(*itr)->timeLeft = pReenqueueElements_[*itr];
				itr = pTimerQueue_.erase(pTimerQueue_.begin());  // 消す
				EnqueueTimer(pElement);  // 時間セットしてエンキュー
				break;
			}
			// 使いまわさないなら
			{
				delete (*itr);  // 解放する
				itr = pTimerQueue_.erase(pTimerQueue_.begin());  // 消す
			}
			if (itr == pTimerQueue_.end())
			{
				break;  // 後続がない = キューが空なら離脱
			}
			(*itr)->timeLeft += diff;  // 差分を後続に反映
			continue;  // 後続も終了している可能性があるため継続
		}
	}
}

mtgb::Timer::Timer()
{
	pInstance_ = this;
}

mtgb::Timer::~Timer()
{
	for (auto& pTimer : Instance().pTimerQueue_)
	{
		delete pTimer;
	}
}

void mtgb::Timer::EnqueueTimer(QUEUE_ELEMENT* _pElement)
{
	float lefter{ _pElement->timeLeft };  // 減算用
	float righter{ 0 };     // 加算用
	// 適切な挿入ポイントを見つける
	for (auto itr = Instance().pTimerQueue_.begin();
		itr != Instance().pTimerQueue_.end();
		itr++)
	{
		if (lefter <= righter + (*itr)->timeLeft)
		{
			_pElement->timeLeft = lefter - righter;
			itr = Instance().pTimerQueue_.insert(itr, _pElement);
			itr++;
			(*itr)->timeLeft -= _pElement->timeLeft;
			return;
		}
		righter += (*itr)->timeLeft;
	}
	// 見つからなかったら末端に追加
	Instance().pTimerQueue_.push_back(_pElement);
}

mtgb::Timer* mtgb::Timer::pInstance_{ nullptr };
