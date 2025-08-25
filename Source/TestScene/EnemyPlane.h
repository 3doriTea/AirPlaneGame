#pragma once
#include <mtgb.h>
#include "../Health.h"

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
	Collider* pCollider_;
	Transform* pTransform_;
	Transform* pTarget_;
	
	Health health_;   // ‘Ì—Í
	float speed_;  // ˆÚ“®‘¬“x m/s
	bool broken_;  // ‰ó‚ê‚Ä‚¢‚é‚©
};
