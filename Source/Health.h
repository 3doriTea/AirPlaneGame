#pragma once

namespace
{
	const int DEFAULT_MAX_VALUE{ 100 };
	const int DEFAULT_INIT_VALUE{ 100 };
}

/// <summary>
/// 体力クラス
/// </summary>
class Health
{
public:
	Health(
		const int _initValue = DEFAULT_INIT_VALUE,
		const int _maxValue = DEFAULT_MAX_VALUE);
	~Health() {}

	/// <summary>
	/// 最大体力値を設定する
	/// </summary>
	/// <param name="_max"></param>
	void SetMax(const int _max);

	/// <summary>
	/// ダメージを与える
	/// </summary>
	/// <param name="_damage">ダメージ値</param>
	void Damage(const int _damage);
	/// <summary>
	/// 回復する
	/// </summary>
	/// <param name="_heal">回復値</param>
	void Heal(const int _heal);

	/// <summary>
	/// 現在の体力を取得
	/// </summary>
	/// <returns>体力値</returns>
	int GetValue() const;
	/// <summary>
	/// 死んでいるかを取得
	/// </summary>
	/// <returns>死んでいる true / false</returns>
	bool IsDead() const;
private:
	int value_;  // 体力値
	int max_;  // 最大の体力値
};
