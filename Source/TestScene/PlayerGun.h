#pragma once
#include <mtgb.h>

#include "../Source/TargetingSystem.h"

class PlayerGun
{
public:
	PlayerGun(mtgb::WindowContext _wc, mtgb::GameObjectLayer _layer, mtgb::Transform* _transform, float _lockonside);
	~PlayerGun();
	void Update(); // クールタイム制御の更新処理
	void Fire(); // 撃つボタンが押されているときの処理（内部で打てない間は無視する処理）
	void Draw();
private:
	float cooldownTimer_; // 撃てない間の時間カウント用
	float cooldownCount_;
	TargetingSystem targetingSystem_;
};