#pragma once
#include <mtgb.h>


struct TargetingSystem;

class PlayerPilot : public mtgb::GameObject
{
public:
	PlayerPilot(const EntityId _plane);
	~PlayerPilot();

	void Update() override;
	void Draw() const override;

private:
	TargetingSystem* pTargetingSystem_;
	Transform* pTransform;
};
