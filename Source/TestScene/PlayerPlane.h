#pragma once
#include <mtgb.h>

class PlayerPlane : public mtgb::GameObject
{
public:
	PlayerPlane();
	~PlayerPlane();

	void Update() override;
	void Draw() const override;

private:
	RigidBody* pRB_;
	Transform* pTransform_;
};
