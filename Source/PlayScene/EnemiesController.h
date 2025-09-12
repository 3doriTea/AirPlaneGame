#pragma once
#include <mtgb.h>

class EnemiesController : public GameObject
{
//public:

public:
	EnemiesController(const EntityId playerId_);
	~EnemiesController();

	void Update() override;

	/// <summary>
	/// 敵を1体出現させる
	/// </summary>
	/// <param name="_worldPosition">スポーンさせるワールド座標</param>
	void Spawan(const Vector3 _worldPosition);

	/// <summary>
	/// プレイヤーのワールド座標を取得する
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetPlayerPosition() const { return playerPosition_; }

private:
	Transform* pPlayerTransform_;
	Vector3 playerPosition_;
};
