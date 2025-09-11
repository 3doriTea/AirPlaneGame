#include "Gun.h"

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
	GameObject::Instantiate<Bullet>(_position, _quaternion, setting_.bulletType);
	oneShotTimeLeft_ += setting_.oneShotTimeSec;
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
