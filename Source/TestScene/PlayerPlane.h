#pragma once
#include <mtgb.h>
#include "../VVPlayer.h"

class PlayerPlane : public mtgb::GameObject
{
public:
	PlayerPlane();
	~PlayerPlane();

	void Update() override;
	void Draw() const override;

	Quaternion RemoveZRotation(Quaternion _q) const;

private:
	int number;
	RigidBody* pRB_;
	Transform* pTransform_;

	VVPlayer vVPlayer_;
};
