#pragma once
#include "ISystem.h"
#include "cmtgb.h"

typedef union _LARGE_INTEGER LARGE_INTEGER;

namespace mtgb
{
	/// <summary>
	/// ゲーム内時間のシステムクラス
	/// </summary>
	class Time : public ISystem
	{
	public:
		Time();
		~Time();

		void Initialize() override;
		void Update() override;

		static inline const double DeltaTime() { return deltaTime_; }
		static inline const float DeltaTimeF() { return static_cast<const float>(deltaTime_); }

	private:
		static const LONGLONG SEC_TO_MICRO;  // マイクロ秒を１秒に変換する
		static const double MICRO_TO_SEC;  // 1秒をマイクロ秒に変換する
		static double deltaTime_;  // フレーム間時間 (秒)

	private:
		LARGE_INTEGER current_;  // 現在のCPU時間 (マイクロ秒)
		LARGE_INTEGER previous_;  // 前回のCPU時間 (マイクロ秒)
	};
}
