#pragma once
#include <mtgb.h>
#include "../VVPlayer.h"

class Radar;

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
	Collider* pCollider_;
	Transform* pTransform_;
	FBXModelHandle hModel_;
	VVPlayer vVPlayer_;
};
