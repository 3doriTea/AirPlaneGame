#pragma once
#include <mtgb.h>
#include "../ProjectTile.h"

/// <summary>
/// 銃器の管理システム
/// </summary>
class Gun
{
public:
	struct Setting
	{
		float oneShotTimeSec;               // 1発撃ったあとの待機時間(秒)
		float reloadTimeSec;                // リロード中の待機時間(秒)
		int bulletCount;                    // リロードまでに撃てる弾数
		ProjectTile::Shooter bulletType;    // 弾の種類（誰が撃つか）
		ProjectTile::Type projectileType;   // 発射体の種類（BulletかMissile）
	};
public:
	Gun(Setting&& _setting);
	~Gun() {}

	/// <summary>
	/// 銃器の更新処理
	/// </summary>
	void Update();
	
	/// <summary>
	/// 撃つ
	/// </summary>
	void Shot(const Vector3& _position, const Quaternion& _quaternion);
	
	/// <summary>
	/// 発射（ミサイル用 - ターゲット指定）
	/// </summary>
	void Shot(const Vector3& _position, const Quaternion& _quaternion,Transform* _target);

	/// <summary>
	/// リロードする
	/// </summary>
	void Reload();

	/// <summary>
	/// リロード中か
	/// </summary>
	/// <returns>リロードである true / false</returns>
	const bool IsReloading() const { return reloadTimeLeft_ >= 0.0f; }
	/// <summary>
	/// 1発撃ったあとのクールタイムか
	/// </summary>
	/// <returns>クールタイムである true / false</returns>
	const bool IsOneShotCooling() const { return oneShotTimeLeft_ >= 0.0f; }

	const bool IsNeedReload() const { return shotCountLeft_ <= 0; }

private:
	void ShotImpl(const Vector3& _position, const Quaternion& _quaternion, Transform* _target);
	Setting setting_;
	float oneShotTimeLeft_;  // 1発撃ったあとの待機時間(秒)
	float reloadTimeLeft_;   // リロード中の待機時間(秒)
	int shotCountLeft_;  // 撃てる回数
};
