#pragma once
#include <mtgb.h>

class Radar;
struct TargetingSystem;

class PlayerGunner : public mtgb::GameObject
{
public:
	PlayerGunner(const EntityId _plane);
	~PlayerGunner();

	void Update() override;
	void Draw() const override;
	
	void SetRadarUI(Radar* _pRadarUI) { pRadarUI_ = _pRadarUI; }
private:
	TargetingSystem* pTargetingSystem_;
	Transform* pTransform_;
	Transform* pPlaneTransform_;
	float angleX_;
	float angleY_;
	Radar* pRadarUI_;
};
