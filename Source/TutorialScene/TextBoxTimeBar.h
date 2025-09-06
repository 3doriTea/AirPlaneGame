#pragma once
#include <mtgb.h>

/// <summary>
/// 時間経過と共に中心に収束するバー
/// </summary>
class TextBoxTimeBar : public mtgb::GameObject
{
public:
	TextBoxTimeBar();
	~TextBoxTimeBar();

	void Update() override;
	void Draw() const override;

	/// <summary>
	/// 残り時間を設定、自動でスタートする
	/// </summary>
	/// <param name="timeLeftMax_">残り時間 (秒)</param>
	void SetTimeLeftMax(const float _timeSec);

private:
	float timeLeft_;     // 現在の残り時間
	float timeLeftMax_;  // 設定された残り時間
};
