#pragma once
//#include <mtgb.h>
#include "ProjectTile.h"

class Bullet : public ProjectTile
{
public:
	///// <summary>
	///// ’e‚Ìí—Ş
	///// </summary>
	//enum struct Shooter
	//{
	//	Enemy,   // “G‚Ì’e
	//	Player,  // ƒvƒŒƒCƒ„[‚Ì’e
	//};

	//enum struct Type
	//{
	//	Bullet,
	//	Missile,
	//};
public:
	Bullet(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter);
	Bullet(const Transform& _shooterTransform, Shooter _shooter);
	~Bullet();

	void Update() override;
	void Draw() const override;

	static const float GetMoveSpeed();
};
