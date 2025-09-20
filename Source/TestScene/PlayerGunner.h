#pragma once
#include <mtgb.h>

class Radar;
//struct TargetingSystem;
class PlayerGun;

class PlayerGunner : public mtgb::GameObject
{
public:
	PlayerGunner(const EntityId _plane);
	~PlayerGunner();

	void Update() override;
	void Draw() const override;
	
	void SetRadarUI(Radar* _pRadarUI) { pRadarUI_ = _pRadarUI; }
private:
//	TargetingSystem* pTargetingSystem_;
	PlayerGun* pPlayerGun_;
	Transform* pTransform_;
	Transform* pPlaneTransform_; // PlayerGunner‚ªæ‚é”òs‹@
	float angleX_;
	float angleY_;
	Radar* pRadarUI_;
};
