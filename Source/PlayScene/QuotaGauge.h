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
	// ノルマゲージの矩形
	RectF quotaGaugeRect_;

	// ゲージの左端から現在のスコアまでのバー
	ImageHandle currentScoreBarImage_;
	RectF currentScoreBarRect_;

	// 現在のスコアからノルマまでのバー
	ImageHandle toQuotaBarImage_;
	RectF toQuotaBarRect_;

	// ノルマからゲージの右端までのバー
	ImageHandle afterQuotaBarImage_;
	RectF afterQuotaBarRect_;
};