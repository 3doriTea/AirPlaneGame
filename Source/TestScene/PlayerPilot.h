#pragma once
#include <mtgb.h>

class PlayerPilot : public mtgb::GameObject
{
public:
	PlayerPilot(const EntityId _plane);
	~PlayerPilot();

	void Update() override;
	void Draw() const override;

private:
	//ロックオンの正方形の一辺の長さ
	int lockOnSide_;
	Vector2Int rectCenter_;
	RectInt lockOnRect_;
	float rockOnDistance_;//前方幾つまでロックオン有効か
	void LockOnShoot();
	Transform* pTransform;
	ImageHandle hImage_;
};
