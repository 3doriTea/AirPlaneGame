#pragma once
#include <mtgb.h>

class EnemyPlane : public GameObject
{
public:
	EnemyPlane(const Vector3& _worldPosition);
	~EnemyPlane();

	void Update() override;
	void Draw() const override;

private:
	FBXModelHandle hModel_;
	RigidBody* pRB_;
	Transform* pTransform_;
	float speed_;
};
