#pragma once
#include <mtgb.h>

class PlayerBullet : public mtgb::GameObject
{
public:
	PlayerBullet(const Vector3& _position, const Quaternion& _quaternion);
	~PlayerBullet();

	void Update() override;
	void Draw() const override;

private:
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	Collider* pCollider_;

	FBXModelHandle hModel_;
	FBXModelHandle hModelCollider_;
};
