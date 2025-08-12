#pragma once
#include <mtgb.h>

class EnemyPlane : public GameObject
{
public:
	EnemyPlane(const Vector3& _worldPosition, const EntityId _playerPlane);
	~EnemyPlane();

	void Update() override;
	void Draw() const override;

private:
	FBXModelHandle hModel_;
	RigidBody* pRB_;
	Transform* pTransform_;
	Transform* pTarget_;
	TransformProxy* pTransformProxy_;
	TransformGuizmo* pTransformGuizmo_;
	float speed_;
};
