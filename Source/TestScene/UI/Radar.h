#pragma once
#include <mtgb.h>
#include <vector>

class Radar : public mtgb::GameObject
{
public:
	Radar(const EntityId _playerId, const GameObjectLayer _layer);
	~Radar();

	void Update() override;
	void Draw() const override;

	void SetViewAngle(const float _angle) { viewAngle_ = _angle; }

	struct Mark2D
	{
		Vector2Int pos;
		float angle;
	};
private:
	float viewAngle_;

	ImageHandle hBack_;  // レーダー盤の画像
	ImageHandle hInView_;  // レーダー視野の画像
	ImageHandle hFrame_;  // レーダー補助線の画像
	ImageHandle hEnemyArrow_;
	ImageHandle hMissileArrow_;
	Transform* pPlayerTransform_;
	std::vector<Mark2D> enemyMarkPos_;
	std::vector<Mark2D> missileMarkPos_; // 敵のミサイルの位置
};
