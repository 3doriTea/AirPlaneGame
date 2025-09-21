#pragma once
#include <mtgb.h>

class QuotaGauge : public mtgb::GameObject
{
public:
	QuotaGauge();
	~QuotaGauge();

	void Update() override;
	void Draw() const override;

	/// <summary>
	/// ポイントを加算する
	/// </summary>
	/// <param name="_point">加算するポイント</param>
	void AddPoint(const int _point);
private:
	ImageHandle hImageBackNormal_;  // 裏ノルマ未達成
	ImageHandle hImageBackClear_;  // 裏ノルマ達成
	ImageHandle hImageFill_;  // 埋まったセル
	ImageHandle hImageFillGood_;  // 埋まったセル(ノルマ越し)
	ImageHandle hImageAir_;  // 空っぽのセル

	int currentPoint_;  // 現在のポイント
};