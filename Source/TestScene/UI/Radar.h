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

private:
	float viewAngle_;

	ImageHandle hBack_;  // レーダー盤の画像
	ImageHandle hInView_;  // レーダー視野の画像
	ImageHandle hFrame_;  // レーダー補助線の画像
	Transform* pPlayerTransform_;
	std::vector<Vector2Int> enemyMarkPos_;
	std::vector<Vector2Int> missileMarkPos_; // 敵のミサイルの位置
};
