#pragma once
#include <mtgb.h>

class QuotaGauge : public mtgb::GameObject
{
public:
	QuotaGauge();
	~QuotaGauge();

	void Update() override;
	void Draw() const override;
private:
	//// ノルマゲージの矩形
	//RectF quotaGaugeRect_;

	//// ゲージの左端から現在のスコアまでのバー
	//RectF currentScoreBarRect_;

	//// 現在のスコアからノルマまでのバー
	//RectF toQuotaBarRect_;

	//// ノルマからゲージの右端までのバー
	//RectF afterQuotaBarRect_;

	const float SCORE_MAX;
};