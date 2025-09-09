#pragma once
#include <mtgb.h>

class EnemiesController : public GameObject
{
//public:

public:
	EnemiesController();
	~EnemiesController();

	/// <summary>
	/// 敵を1体出現させる
	/// </summary>
	/// <param name="_worldPosition">スポーンさせるワールド座標</param>
	void Spawan(const Vector3 _worldPosition);

private:
};
