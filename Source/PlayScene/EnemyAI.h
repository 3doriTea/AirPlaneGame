#pragma once
#include <functional>
#include "MTStat/MTStat.h"

/// <summary>
/// 敵のAI(しょぼ) 部品クラスとして機能
/// </summary>
class EnemyAI
{
public:
	/// <summary>
	/// AIへの入力データ
	/// </summary>
	struct InputData
	{

	};

	/// <summary>
	/// メインステート
	/// </summary>
	enum MAIN_STATE
	{
		S_SLEEP,  // 範囲外待機
		S_FREE,   // 自由行動
		S_FIGHT,  // 戦闘中
	};

public:
	EnemyAI();
	~EnemyAI() {};

	/// <summary>
	/// AIの更新処理をする
	/// </summary>
	void Update();

	/// <summary>
	/// 入力データをセットする
	/// </summary>
	/// <param name="_data">AIの更新時に参照されるデータ</param>
	static void SetInput(InputData&& _data);
private:
	mtstat::MTStat<MAIN_STATE> state_;  // 状態

	static InputData inputData_;  // 入力データ
};
