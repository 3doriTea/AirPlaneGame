#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"

Enemy::Enemy() : GameObject(GameObjectBuilder()
	.SetName("enemy")
	.SetPosition(Vector3(0, 0, 10))
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() }
{
	hModel_ = OBJ::Load("Model/OBJ/enemy.obj");
	pTransform_->position_.z = 6.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
}

void Enemy::Draw() const
{
	Draw::OBJModel(hModel_, pTransform_);
}
