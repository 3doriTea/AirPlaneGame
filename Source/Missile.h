#pragma once
#include "ProjectTile.h"

class Missile : public ProjectTile
{
public:
	Missile(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter, Transform* _target);
	Missile(const Transform& _shooterTransform, Transform* _target, const Shooter _shooter);
	void Update() override;
	void Draw() const override;
private:
	Transform* pTarget_;
};