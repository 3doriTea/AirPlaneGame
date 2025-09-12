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
		void Update() override;

		/// <summary>
		/// スコアを増加する
		/// </summary>
		static void AddScore(int32_t _score);

		/// <summary>
		/// 現在のスコアを取得する
		/// </summary>
		static int32_t GetScore();

		/// <summary>
		/// ノルマのスコアを取得する
		/// </summary>
		/// <returns></returns>
		static int32_t GetQuotaScore();
		/// <summary>
		/// スコアをリセットする
		/// </summary>
		static void ResetScore();

		/// <summary>
		/// ノルマを達成したか
		/// </summary>
		/// <returns>達成しているなら真を返す</returns>
		static bool AchievedQuota();
	private:
		static int32_t score_;
		// ノルマのスコア
		static int32_t quotaScore_;
	};
}