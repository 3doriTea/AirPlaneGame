#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"

Enemy::Enemy(const Vector3& _position) : GameObject(GameObjectBuilder()
	.SetName("enemy")
	.SetPosition(_position)
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() },
	hp_{ 100 },
	radius_{ 30 },
	speed_{ 5.0f }
{
	//hModel_ = OBJ::Load("Model/OBJ/enemy.obj");
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	//hModel_ = Fbx::Load("Assets/Model/gCube.fbx");
	massert(hModel_ >= 0 && "“Gƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");
	pTransform_->position_.z = 30.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
}

void Enemy::Draw() const
{
	//Draw::OBJModel(hModel_, pTransform_);
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
