#pragma once
#include "ISystem.h"
#include <cstdint>

namespace mtgb
{
	class ScoreManager : public ISystem
	{
	public:
		ScoreManager();
		~ScoreManager();
		void Initialize() override {}
		void Update() override {}

		/// <summary>
		/// スコアを増加する
		/// </summary>
		static void AddScore(int32_t _score);

		/// <summary>
		/// 現在のスコアを取得する
		/// </summary>
		static int32_t GetScore() { return score_; }

		/// <summary>
		/// スコアをリセットする
		/// </summary>
		static void ResetScore() { score_ = 0; }

	private:
		static int32_t score_;
	};
}