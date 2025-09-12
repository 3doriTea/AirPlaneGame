#pragma once
#include <mtgb.h>
#include "MTStat/MTStat.h"

/// <summary>
/// チュートリアルからプレイシーンに遷移するまでのタイマー
/// </summary>
class ToPlayTimer : public mtgb::GameObject
{
public:
	/// <summary>
	/// アニメーション状態
	/// </summary>
	enum STATE
	{
		S_ENTER,  // でてくる
		S_STAY,  // タイマー表示
		S_OVER,  // 隠す
		S_EXIT,  // ひっこむ
		S_MAX,  // アニメーション状態の種類数
	};

public:
	/// <summary>
	/// プレイシーンに遷移するまでのタイマー
	/// </summary>
	/// <param name="_timeSec">遷移までの時間</param>
	ToPlayTimer(const float _timeSec);
	~ToPlayTimer();

	void Update() override;
	void Draw() const override;

private:
	float timeLeft_;
	float totalTimeLeft_;
	ImageHandle hBackImageShow_;
	ImageHandle hBackImageHide_;
	mtstat::MTStat<STATE> stat_;
};
