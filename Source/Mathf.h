#pragma once
#include <random>
#include <cstdint>

/// <summary>
/// 32bit浮動小数点数の数学ユーティリティ
/// </summary>
namespace wtgb::Mathf
{
	/// <summary>
	/// 乱数を作るやつ
	/// </summary>
	class Randomer
	{
	public:
		/// <summary>
		/// シード値指定
		/// </summary>
		/// <param name="_seed">シード値</param>
		Randomer(const uint32_t _seed);
		/// <summary>
		/// シード値を本当のランダムに
		/// </summary>
		Randomer();
		~Randomer();

		/// <summary>
		/// 次の乱数を生成する
		/// </summary>
		/// <param name="_max">最大値</param>
		/// <returns>乱数整数値</returns>
		int NextInt(int _max = INT_MAX);

		/// <summary>
		/// 乱数を0～1の範囲で取得する
		/// </summary>
		/// <returns>0..1</returns>
		float Rand();

	private:
		std::mt19937 randomEngine_;  // 乱数エンジン
	};
}
