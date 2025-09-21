#include "Gun.h"
#include "../Bullet.h"
#include "../Missile.h"

Gun::Gun(Setting&& _setting) :
	setting_{ std::move(_setting) },
	preShotWaitTimeLeft_{},
	lockOnTimeLeft_{},
	isLockingOn_{ false },
	oneShotTimeLeft_{},
	reloadTimeLeft_{},
	shotCountLeft_{}
{
	// 最初に弾を詰めてあげる
	shotCountLeft_ = setting_.bulletCount;
	// ロックオンタイマーを初期化
	lockOnTimeLeft_ = setting_.lockOnTimeSec;
}

void Gun::Update()
{
	const float DT = Time::DeltaTimeF();

	if (preShotWaitTimeLeft_ > 0.0f)
	{
		preShotWaitTimeLeft_ -= DT;
	}
	if (oneShotTimeLeft_ >= 0.0f)
	{
		oneShotTimeLeft_ -= DT;
	}
	if (reloadTimeLeft_ >= 0.0f)
	{
		reloadTimeLeft_ -= DT;
	}

	// ロックオンタイマーの更新
	if (isLockingOn_ && lockOnTimeLeft_ > 0.0f)
	{
		lockOnTimeLeft_ -= DT;
	}
}

void Gun::Shot(const Vector3& _position, const Quaternion& _quaternion)
{
	ShotImpl(_position, _quaternion, nullptr);
}

void Gun::Shot(const Vector3& _position, const Quaternion& _quaternion, Transform* _target)
{
	ShotImpl(_position, _quaternion, _target);
}

void Gun::StartLockOnCountdown()
{
	if (!isLockingOn_)
	{
		isLockingOn_ = true;
		lockOnTimeLeft_ = setting_.lockOnTimeSec;
	}
}

void Gun::ResetLockOnCountdown()
{
	isLockingOn_ = false;
	lockOnTimeLeft_ = setting_.lockOnTimeSec;
}

bool Gun::IsLockOnComplete() const
{
	return isLockingOn_ && lockOnTimeLeft_ <= 0.0f;
}

bool Gun::IsLockingOn() const
{
	return isLockingOn_;
}

float Gun::GetLockOnProgress() const
{
	if (!isLockingOn_ || setting_.lockOnTimeSec <= 0.0f)
	{
		return 0.0f;
	}
	return 1.0f - (lockOnTimeLeft_ / setting_.lockOnTimeSec);
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
			GameObject::Instantiate<Missile>(_position, _quaternion, setting_.bulletType, _target);
		else
			GameObject::Instantiate<Bullet>(_position, _quaternion, setting_.bulletType); // fallback
		break;
	}

	oneShotTimeLeft_ = setting_.oneShotTimeSec;

	// 射撃後はロックオンをリセット
	ResetLockOnCountdown();
}
