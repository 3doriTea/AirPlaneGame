#pragma once
#include <mtgb.h>
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
		Vector3 playerPos;
		Transform* pSelfTrans{ nullptr };
	};
	/// <summary>
	/// AIからの出力データ
	/// </summary>
	struct OutData
	{
		bool isActive;
		bool isFire;
		bool isRound;
		Vector3 lookPosition;
	};

	/// <summary>
	/// メインステート
	/// </summary>
	enum MAIN_STATE
	{
		S_SLEEP,   // 範囲外待機
		S_SEARCH,  // 索敵行動
		S_FIGHT,   // 戦闘行動
	};
	/// <summary>
	/// 戦闘行動ステート
	/// </summary>
	enum FIGHT_STATE
	{
		SF_LOOK_AT_PLAYER,  // プレイヤーに向ける
		SF_AVOID,           // プレイヤーに衝突しないように避ける
		SF_ROUND,           // Z軸でぐるぐる回る
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
	inline void SetInputData(InputData&& _data) { input_ = std::move(_data); }
	/// <summary>
	/// 出力データをゲットする
	/// </summary>
	/// <returns>出力データの参照</returns>
	inline const OutData& GetOutData() const { return out_; }

private:  // 内部で使う便利系メンバ関数
	/// <summary>
	/// プレイヤーまでの距離を取得
	/// </summary>
	/// <returns>プレイヤーまでの距離</returns>
	const float GetToPlayerDistance() const;

	/// <summary>
	/// プレイヤーが前方向にいるか
	/// </summary>
	/// <returns>前方向にいる true / false</returns>
	const bool IsForwardToPlayerDir() const;

private:
	mtstat::MTStat<MAIN_STATE> sMain_;    // 主な状態
	mtstat::MTStat<FIGHT_STATE> sFight_;  // 戦闘時の状態

	InputData input_;  // 入力データ
	OutData out_;      // 出力データ
};
