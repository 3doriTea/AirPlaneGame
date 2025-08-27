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
	void Search();
private:
	FBXModelHandle hModel_;
	RigidBody* pRB_;
	Collider* pCollider_;
	Transform* pTransform_;
	Transform* pTarget_;

	Health health_;   // 体力
	float speed_;  // 移動速度 m/s
	bool broken_;  // 壊れているか
	float lockOnAngle_;// ロックオン、プレイヤーを発見する角度(degree)
	float lockOnDistance_;// ロックオン、プレイヤーを発見する距離

};
