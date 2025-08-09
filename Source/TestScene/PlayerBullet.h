#pragma
#include <mtgb.h>

//#define TEMPLATE_GAMEOBJECT_H(NAME)\
//\
//class NAME : public mtgb::GameObject\
//{\
//public:\
//	NAME();\
//	~NAME();\
//\
//	void Update() override;\
//	void Draw() const override;\
//\
//private:\
//	Transform* pTransform_;\
//	RigidBody* pRigidBody_;\
//};\
//
//
//TEMPLATE_GAMEOBJECT_H(PlayerBullet)

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
};
