#include "Bullet.h"

using namespace mtgb;

namespace
{
	const float BULLET_SPEED{ 100.f }; // 1秒当たりの移動ピクセルスピード
}

Bullet::Bullet(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter)
	: ProjectTile(_position, _quaternion, _shooter, Type::Bullet)
{
	InitCommon();
}

Bullet::Bullet(const Transform& _shooterTransform, Shooter _shooter)
	: ProjectTile(_shooterTransform, _shooter, Type::Bullet)
{
	InitCommon();
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	pRb_->velocity_ = pTransform_->Forward() * BULLET_SPEED;
}

void Bullet::Draw() const
{
	ProjectTile::Draw(); // 基底クラスの描画処理を呼び出し
}

const float Bullet::GetMoveSpeed()
{
	return BULLET_SPEED;
}

void Bullet::InitCommon()
{
	SetName("Bullet");
	hModel_ = Fbx::Load("Model/NewBullet.fbx");
	massert(hModel_ >= 0 && "弾のモデルの読み込みに失敗");

	// 3秒経ったら消す
	Timer::AddAram(3.0f, [this] { DestroyMe(); });

	Audio::PlayOneShotFile("Sound/Effect/shot.wav");
}
