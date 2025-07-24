#pragma once

/// <summary>
/// レートを扱うバー
/// </summary>
class Bar
{
public:
	Bar();
	~Bar();

	/// <summary>
	/// 現在のレートを取得
	/// </summary>
	/// <returns>0.0f ~ 0.1f</returns>
	const float GetRate() const;

	/// <summary>
	/// 最大値をセットする
	/// </summary>
	/// <param name="_maxValue">最大値</param>
	inline void SetMax(const float _maxValue) { maxValue_ = _maxValue; CalculateSafeValue(); }
	/// <summary>
	/// 最小値をセットする
	/// </summary>
	/// <param name="_minValue">最小値</param>
	inline void SetMin(const float _minValue) { minValue_ = _minValue; CalculateSafeValue(); }
	/// <summary>
	/// 現在の値をセットする
	/// </summary>
	/// <param name="_currentValue"></param>
	inline void SetCurrent(const float _currentValue) { currentValue_ = _currentValue; CalculateSafeValue(); }

private:
	/// <summary>
	/// 現在の値を範囲内にするよう計算する
	/// </summary>
	void CalculateSafeValue();

private:
	float currentValue_;  // 現在の値
	float maxValue_;  // 最大値
	float minValue_;  // 最小値
};
