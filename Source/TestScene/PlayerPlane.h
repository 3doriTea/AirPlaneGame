#pragma once
#include <mtgb.h>

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
};
