#include "ProjectTile.h"
#include "Missile.h"
#include "Bullet.h"

using namespace mtgb;

namespace
{
	const float PROJECTILE_SPEED{ 100.f };
}

ProjectTile::ProjectTile(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter, const Type _projectileType) 
	: GameObject(GameObjectBuilder()
		.SetName("ProjectTile")
		.SetPosition(_position)
		.SetRotate(_quaternion)
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() },
	pRb_{ Component<RigidBody>() },
	pCollider_{ Component<Collider>() },
	shooter_{ _shooter },
	projectileType_{ _projectileType },
	hModel_{ INVALID_HANDLE }
{
	if (_shooter == Shooter::Enemy)
	{
		tag_ = GameObjectTag::Enemy;
	}
	else if (_shooter == Shooter::Player)
	{
		tag_ = GameObjectTag::PlayerPlane;
	}
	// ‹¤’Ê‚ÌColliderÝ’è
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->SetCenter(Vector3::Zero());
	pCollider_->SetRadius(0.5f);

	// 3•bŒo‚Á‚½‚çÁ‚·
	//Timer::AddAram(3.0f, [this] { DestroyMe(); });
}

ProjectTile::ProjectTile(const Transform& _shooterTransform, Shooter _shooter, const Type _projectileType) 
	: ProjectTile(_shooterTransform.position,_shooterTransform.rotate,_shooter,_projectileType)
{
	pTransform_->scale = _shooterTransform.scale;
}

ProjectTile::~ProjectTile()
{
}

void ProjectTile::Draw() const
{
	if (hModel_ != INVALID_HANDLE)
	{
		Draw::FBXModel(hModel_, *pTransform_, 0);
	}
	pCollider_->Draw();
}

