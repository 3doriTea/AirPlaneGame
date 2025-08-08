#pragma once
#include <mtgb.h>

class PlayerGunner : public mtgb::GameObject
{
public:
	PlayerGunner(const EntityId _plane);
	~PlayerGunner();

	void Update() override;
	void Draw() const override;

private:
	Transform* pTransform_;
};