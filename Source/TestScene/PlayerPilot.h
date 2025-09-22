#pragma once
#include <mtgb.h>


class PlayerGun;

class PlayerPilot : public mtgb::GameObject
{
public:
	PlayerPilot(const EntityId _plane);
	~PlayerPilot();

	void Update() override;
	void Draw() const override;

private:
	PlayerGun* pPlayerGun;
	Transform* pTransform;
};
