#pragma once
#include <mtgb.h>
#include <vector>
#include "../Source/TextBox.h"

class OrderText : public mtgb::GameObject
{
public:

	enum EnemyStatus
	{
		NoEnemy,
		NearEnemy,
	};

	// 状況に合わせて、テキストを表示させたい（例えば、敵をロックオンしたり、近くに敵が居たりする場合）
	OrderText(const EntityId _playerId, const GameObjectLayer _layer);
	~OrderText();

	void Update() override;
	void Draw() const override;

private:
	std::vector<std::string> orderTexts_;
	EnemyStatus enemyStatus_;
	TextBox* pTextBox_;
	Transform* pPlayerTransform_;
};