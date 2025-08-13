#pragma once
#include <mtgb.h>
#include <vector>

class Radar : public mtgb::GameObject
{
public:
	Radar(const EntityId _playerId);
	~Radar();

	void Update() override;
	void Draw() const override;

private:
	ImageHandle hBack_;  // ���[�_�[�Ղ̉摜
	Transform* pPlayerTransform_;
	std::vector<Vector2Int> enemyMarkPos_;
};
