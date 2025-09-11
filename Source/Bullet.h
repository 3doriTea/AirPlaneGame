#pragma once
#include <mtgb.h>

class Bullet : public mtgb::GameObject
{
public:
	/// <summary>
	/// ’e‚Ìí—Ş
	/// </summary>
	enum struct Type
	{
		Enemy,   // “G‚Ì’e
		Player,  // ƒvƒŒƒCƒ„[‚Ì’e
	};

public:
	Bullet(const Vector3& _position, const Quaternion& _quaternion, const Type _bulletType);
	~Bullet();

	/// <summary>
	/// ’e‚Ìí—Ş‚ğæ“¾
	/// </summary>
	/// <returns>’e‚Ìí—Ş</returns>
	const Type GetType() const { return type_; }

	void Update() override;
	void Draw() const override;

private:
	Transform*     pTransform_;
	RigidBody*     pRb_;
	Collider*      pCollider_;

	FBXModelHandle hModel_;
	Type           type_;
};
