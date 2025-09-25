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
		/// スコアの加点を行う
		/// </summary>
		static void AddScore(uint32_t _score);
		/// <summary>
		/// ノルマの加点を行う
		/// </summary>
		static void AddQuota(uint32_t _quota) { ScoreManager::quotaScore_ += _quota; }

		/// <summary>
		/// 現在のスコアを取得する
		/// </summary>
		static int32_t GetScore();

		/// <summary>
		/// スコアの減点を行う
		/// </summary>
		/// <param name="_score"></param>
		static void SubtractScore(uint32_t _score);
		/// <summary>
		/// ノルマのスコアを取得する
		/// </summary>
		/// <returns></returns>
		static int32_t GetQuotaScore();
		/// <summary>
		/// スコアの最大値を取得する
		/// </summary>
		/// <returns>スコア値</returns>
		static int32_t GetQuotaScoreMax();
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