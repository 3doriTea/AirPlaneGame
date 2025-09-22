#pragma once
#include <mtgb.h>

class ProjectTile : public mtgb::GameObject
{
public:

	enum struct EventType
	{
		Fired,
		Hit,
		Destroyed,
	};

	enum struct Shooter 
	{
		Enemy,
		Player,
	};

	enum struct Type
	{
		Bullet,
		Missile,
	};

	struct EventData
	{
		EntityId id;
		Shooter shooter;
		Type type;
		EventType eventType;
	};
protected:
	ProjectTile(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter, const Type _projectileType);
	ProjectTile(const Transform& _shooterTransform, Shooter _shooter, const Type _projectileType);
	
public:
	virtual ~ProjectTile();

	virtual void Update() override = 0;
	virtual void Draw() const override;

	/// <summary>
	/// ’e‚ÌŽí—Þ‚ðŽæ“¾
	/// </summary>
	/// <returns>’e‚ÌŽí—Þ</returns>
	const Shooter GetShooter() const { return shooter_; }

	/// <summary>
	/// ”­ŽË‘Ì‚ÌŒ^‚ðŽæ“¾
	/// </summary>
	/// <returns>”­ŽË‘Ì‚ÌŒ^</returns>
	const Type GetProjectileType() const { return projectileType_; }

protected:
	Transform* pTransform_;
	RigidBody* pRb_;
	Collider* pCollider_;

	FBXModelHandle hModel_;
	Shooter shooter_;
	Type projectileType_;
};