#pragma once
#include <mtgb.h>

class PlayerDamageEffect : public GameObject
{
public:
	PlayerDamageEffect();
	~PlayerDamageEffect();

	void Update() override;
	void Draw() const override;

private:
	float timeLeft_;
	ImageHandle hImage_;  // 画面エフェクト画像のハンドル
};
