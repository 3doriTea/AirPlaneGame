#pragma once
#include "ProjectTile.h"

class Missile : public ProjectTile
{
public:
	Missile(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter, Transform* _target);
	Missile(const Transform& _shooterTransform, Transform* _target, const Shooter _shooter);
	~Missile();
	void Update() override;
	void Draw() const override;
private:
	void InitCommon(Shooter _shooter);
	void OnShotDown();
	Transform* pTarget_;
	float timeLeft_;  // ミサイルが消えるまでのカウントダウン
};