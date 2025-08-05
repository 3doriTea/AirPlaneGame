#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"

Enemy::Enemy() : GameObject(GameObjectBuilder()
	.SetName("enemy")
	.SetPosition(Vector3(0, 0, 10))
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() },
	hp_{ 100 },
	radius_{ 30 },
	speed_{ 5.0f }
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	//hModel_ = OBJ::Load("Model/OBJ/enemy.obj");
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	//hModel_ = Fbx::Load("Assets/Model/gCube.fbx");
	massert(hModel_ >= 0 && "“Gƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");
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
	//Draw::OBJModel(hModel_, pTransform_);
	Draw::FBXModel(hModel_, *pTransform_, 0);
	//LOGF("EnemyDraw\n");
}
