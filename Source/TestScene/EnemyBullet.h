#pragma once
#include <mtgb.h>

class EnemyBullet : public mtgb::GameObject
{
public:
	EnemyBullet(const Vector3& _position, const Quaternion& _quaternion);
	~EnemyBullet();

	void Update() override;
	void Draw() const override;
	
private:
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	Collider* pCollider_;

	FBXModelHandle hModel_;
	FBXModelHandle hModelCollider_;
};