#include "Gun.h"
#include "../Bullet.h"
#include "../Missile.h"

Gun::Gun(Setting&& _setting) :
	setting_{ std::move(_setting) },
	oneShotTimeLeft_{},
	reloadTimeLeft_{},
	shotCountLeft_{}
{
	// 最初に弾を詰めてあげる
	shotCountLeft_ = setting_.bulletCount;
}

void Gun::Shot(const Vector3& _position, const Quaternion& _quaternion)
{
	ShotImpl(_position, _quaternion, nullptr);
}

void Gun::Shot(const Vector3& _position, const Quaternion& _quaternion,Transform* _target)
{
	ShotImpl(_position, _quaternion, _target);
}

void Gun::Update()
{
	const float DT{ Time::DeltaTimeF() };

	if (oneShotTimeLeft_ >= 0.0f)
	{
		oneShotTimeLeft_ -= DT;
	}
	if (reloadTimeLeft_ >= 0.0f)
	{
		reloadTimeLeft_ -= DT;
	}
	
}

void Gun::Reload()
{
	// リロード時間をリセット
	reloadTimeLeft_ = setting_.reloadTimeSec;
	// 撃てる弾数もリセット
	shotCountLeft_ = setting_.bulletCount;
	
}

void Gun::ShotImpl(const Vector3& _position, const Quaternion& _quaternion, Transform* _target)
{
	if (IsOneShotCooling())
	{
		return;
	}
	if (IsReloading())
	{
		return;
	}
	

	if (IsNeedReload())
	{
		Reload();
		return;
	}

	shotCountLeft_--;

	// 発射体の種類に応じて生成
	switch (setting_.projectileType)
	{
	case ProjectTile::Type::Bullet:
		GameObject::Instantiate<Bullet>(_position, _quaternion, setting_.bulletType);
		break;
	case ProjectTile::Type::Missile:

		if (_target)
		{
			GameObject::Instantiate<Missile>(_position, _quaternion, setting_.bulletType, _target);
		}
		else
		{
			GameObject::Instantiate<Bullet>(_position, _quaternion, setting_.bulletType);
		}
		break;

	}

	oneShotTimeLeft_ += setting_.oneShotTimeSec;
}
