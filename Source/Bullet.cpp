#include "Bullet.h"

using namespace mtgb;

namespace
{
	const float BULLET_SPEED{ 100 }; // 1•b“–‚½‚è‚ÌˆÚ“®ƒsƒNƒZƒ‹ƒXƒs[ƒh
	Vector3 BULLET_SCALE{ 3, 3, 3 };
	const float BULLET_DESTROY_TIME_SEC{ 3 }; // ¶¬‚³‚ê‚Ä‚©‚ç”jŠü‚³‚ê‚é‚Ü‚Å‚Ì•b”
}

Bullet::Bullet(const Vector3& _position, const Quaternion& _quaternion, const Shooter _shooter)
	: ProjectTile(_position, _quaternion, _shooter, Type::Bullet),
	timeLeft_{ BULLET_DESTROY_TIME_SEC }
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
	timeLeft_ -= Time::DeltaTimeF();
	if (timeLeft_ <= 0.0f)
	{
		DestroyMe();
	}
}

void Bullet::Draw() const
{
	ProjectTile::Draw(); // Šî’êƒNƒ‰ƒX‚Ì•`‰æˆ—‚ðŒÄ‚Ño‚µ
}

const float Bullet::GetMoveSpeed()
{
	return BULLET_SPEED;
}

void Bullet::InitCommon()
{
	SetName("Bullet");
	hModel_ = Fbx::Load("Model/NewBullet.fbx");
	massert(hModel_ >= 0 && "’e‚Ìƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");
	pTransform_->scale *= BULLET_SCALE;
	//Timer::AddAram(BULLET_DESTROY_TIME_SEC, [this] { DestroyMe(); });
	pTransform_->Compute();
	Audio::PlayOneShotFile("Sound/Effect/shot.wav");
}
