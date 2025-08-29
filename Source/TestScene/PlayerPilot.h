#pragma once
#include <mtgb.h>

struct mtgb::LockOn;
class PlayerPilot : public mtgb::GameObject
{
public:
	PlayerPilot(const EntityId _plane);
	~PlayerPilot();

	void Update() override;
	void Draw() const override;

private:

	LockOn* lockOn_;
	//ロックオンの正方形の一辺の長さ
	//float lockOnSide_;
	//float enemyFrameSideExtents_;
	RectF lockOnRect_;
	RectF enemyFrameRect_;
	bool lockOnAny_;
	float lockOnDistance_;//前方幾つまでロックオン有効か
	//void LockOn();
	void Shoot();
	Transform* pTransform;
	ImageHandle lockOnFrame_;
	ImageHandle lockOnReticle_;
	
	UIParams uiParams_;
	RectDetector rectDetector;
	RectContainsInfo* pTargetInfo_;
};
