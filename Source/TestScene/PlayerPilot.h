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
	int enemyFrameSideExtents_;
	Vector2F rectCenter_;
	RectF lockOnRect_;
	RectF enemyFrameRect_;
	bool lockOnAny;
	float lockOnDistance_;//前方幾つまでロックオン有効か
	void LockOn();
	void Shoot();
	Transform* pTransform;
	ImageHandle lockOnFrame_;
	ImageHandle lockOnEnemyFrame_;
	
	UIParams uiParams_;
	RectContainsInfo targetInfo_;
};
